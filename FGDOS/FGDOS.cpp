#include "FGDOS.hpp"

bool FGDOS::updateData() {
    //10 byte buffer, byte 1 is don't care because we are writing address 0 and the rest are the raw data
    etl::array<uint8_t,10> buffer{};

    //read address zero (and continue to the next registers)
    uint8_t baseAddress = 0b1000'0000;


    //error checking here
    if (!writeRead(&baseAddress, 1, buffer.data(), buffer.size())) {
        //return error instead!
        LOG_ERROR << "SPI read/write failed!\n";
        return false;


    }

    uint8_t rchcnt=buffer[2];
    LOG_TRACE << "Read buffer contents:" << buffer.data() << '\n';
    for (int i=1;i<buffer.size();i++) {
        LOG_TRACE << "At :" << i-1 << " = " << static_cast<int>(buffer[i]);
    }

    //contains flags and 2 most significant bits for ref and sensor respectively
    const uint8_t rLast = buffer[6];
    const uint8_t sLast = buffer[9];

    recharging = rchcnt & 0b1000'0000;
    const bool refReady = rLast & 0b1000;
    const bool sensorReady = sLast & 0b1000;
    //LOG_DEBUG<<"Recharge count:"<<rchcnt<<'\n';
    //if recharging no data available
    if(recharging) {
        LOG_DEBUG << "Disregarding data because sensor is recharging\n";
        LOG_DEBUG << "Sensor frequency is:" << frequencyFromRaw(buffer[7] | (buffer[8]<<8) | ((sLast&0b11) << 16));
        return false;
    }

    //store secondary data :ref (if ready) and temp
    if(refReady) {
        refPrevFrequency = refFrequency;
        uint32_t rawRefFrequency = buffer[4] | (buffer[5] << 8) | ((rLast & 0b11) << 16);
        LOG_DEBUG << "Raw reference frequency:" << rawRefFrequency << '\n';

        referenceOverflown = rLast&0b100;
        refFrequency = frequencyFromRaw(rawRefFrequency);
        LOG_DEBUG << "Updated reference frequency to:" << refFrequency << '\n';

    }

    temperature = buffer[1];

    //if sensor not ready, don't store it and return false
    if(!sensorReady) {
    LOG_DEBUG << "Sensor not ready\n";
    return false;
    }

    //bit 7 is zero anyway at this point so no need to clear it
    //if rchcnt=127 then it may be overflown so that value is an error
    //but is dealt with outside the function
    rechargeCount = rchcnt;
    LOG_DEBUG << "Recharge count: " << static_cast<uint32_t>(rchcnt) << '\n';

    if(rchcnt != 0) {
        clearRechargeCount();
    }

    //deal with these errors outside this function
    sensorOverflown = sLast & 0b100;

    sensorPrevFrequency = sensorFrequency;
    uint32_t rawSensorFrequency = buffer[7] | (buffer[8] << 8) | ((sLast & 0b11) << 16);
    LOG_DEBUG << "Raw sensor data:" << rawSensorFrequency << '\n';
    uint32_t uncompensatedSensorFrequency = frequencyFromRaw(rawSensorFrequency);
    LOG_DEBUG << "Uncompensated sensor frequency: " << uncompensatedSensorFrequency << '\n';
    sensorFrequency = temperatureCompensateFrequency(uncompensatedSensorFrequency);

    previousDose += doseIncrease;
    LOG_DEBUG<<"Updated sensor frequency to:"<<sensorFrequency<<'\n';

    updateLastReadTick();
    return true;
}

void FGDOS::calculateDose() {
    if(not enoughTimeElapsed()) {
        if(sensorOverflown) {
            LOG_WARNING << "Sensor counter has overflowed!\n";
            //return error
            return;
        }
        if(referenceOverflown) {
            LOG_WARNING << "Reference counter has overflowed!\n";
            //return error
            return;
        }
        if(rechargeCount == 127) {
            LOG_WARNING << "Recharge count possibly has overflowed!\n";
            //return error
            return;
        }
        LOG_TRACE << "Skipping dose calculation: Not enough time elapsed\n";
        return;
}

    if(not updateData()) {
        LOG_TRACE << "Data not updated\n";
        return;
    }


    if(sensorOverflown) {
        LOG_WARNING << "Sensor counter has overflowed!\n";
        //return error
        return;
    }
    if(referenceOverflown) {
        LOG_WARNING << "Reference counter has overflowed!\n";
        //return error
        return;
    }
    if(rechargeCount == 127) {
        LOG_WARNING << "Recharge count possibly has overflowed!\n";
        //return error
        return;
    }
	//may be negative during recharge,in that case it is corrected by the addition of approximated recharge dose
    int32_t deltaF = static_cast<int32_t>(sensorFrequency) - static_cast<int32_t>(sensorPrevFrequency);
    doseIncrease = static_cast<float>(deltaF) / configSensitivity;

    if(rechargeCount != 0) {
        LOG_DEBUG << "Skipping dose increase correction:" << doseIncrease;
        return;
        doseIncrease += approximateDoseFromRecharges();
}
    LOG_DEBUG << "Dose increase:" << doseIncrease;
	configASSERT(doseIncrease >= 0);
}

uint32_t FGDOS::frequencyFromRaw(const uint32_t freq) const {

	//sensor and ref frequencies for raw counter
    return (freq * configClockFrequency) / windowAmount;

}

uint32_t FGDOS::temperatureCompensateFrequency(const uint32_t freq) const {
    //TODO implement
    return freq;
}

void FGDOS::initConfiguration(const uint8_t chargeVoltage,const bool highSensitivity, const bool forceRecharge) {
        //write so that recharging is disabled
        LOG_DEBUG << "Before initConfig:";
        debugPrintAll();
        constexpr uint8_t recharge_write_address=0b0100'0000 | 0xD;
        etl::array<uint8_t,2> buffer{recharge_write_address, 0};
        if (!write(buffer.data(), buffer.size()))
        {

            //set some sort of error flag
            LOG_ERROR << "SPI write failed!\n";
            return;

        }
        //prepare and write config data
        ConfigData config{};
        config.setTDiv(tDiv);

        config.setSensitivity(highSensitivity);

        //make sure that charge voltage is a 3 bit value
        config.setVoltage(chargeVoltage&0b111);
        uint8_t target5Bits = frequencyTo5Bit(targetFrequency);
        config.setTargetFrequency(target5Bits);
        uint8_t threshold5Bits = frequencyTo5Bit(thresholdFrequency);


        config.setThresholdFrequency(forceRecharge ? target5Bits : threshold5Bits);

        if (!write(config.data.data(),config.data.size())) {
            //set some sort of error flag
            LOG_ERROR << "SPI write failed!\n";
            return;
        }

        //enable recharge
        buffer.back() = config.getRechargeEnableByte();
        if (!write(buffer.data(),buffer.size())) {
            //set some sort of error flag
            LOG_ERROR << "SPI write failed!\n";
            return;
        }





        //if force recharge, write threshold back
        if(forceRecharge) {
        constexpr uint8_t thresholdWriteAddress = writeMask|0xA;
            buffer.front() = thresholdWriteAddress;
            buffer.back() = config.getThresholdByte(threshold5Bits);

            if (!write(buffer.data(),buffer.size()))
            {

                //set some sort of error flag
                LOG_ERROR << "SPI write failed!\n";
                return;
            }
        //TODO set flag to disregard the first recharge
        }
    //just in case recharge count isn't set to 0 by default
    clearRechargeCount();

    //remove this after testing
    LOG_DEBUG << "After initConfig:";
    debugPrintAll();

    //If force recharge is set to false, we may want to read the current frequency which may be below the target and include the radiation received before initialization
    //Should add a function getPastDose()
    if (!updateData())
    {
        LOG_TRACE << "Data not updated after initialization\n";

    }


}

void FGDOS::clearRechargeCount() const {
    constexpr uint8_t write_address = writeMask | 0x01;
    etl::array<uint8_t,2> buffer{write_address, 0};
    if (!write(buffer.data(),buffer.size())) {
        //set some sort of error flag
        LOG_ERROR << "SPI write failed!\n";
        return;
    }

}

void FGDOS::debugPrintAll() const {
    uint8_t read_address=readMask | 0x0;
    etl::array<uint8_t,16> buffer{};
    if (!writeRead(&read_address,1,buffer.data(),buffer.size())) {
        LOG_DEBUG<<"SPI read failed!\n";
        return;
    }
    for (int i=1;i<buffer.size();i++) {
        LOG_DEBUG<<"At address:"<<i-1<<" value is:"<<buffer[i]<<'\n';
    }


}

FGDOS::FGDOS(const uint32_t clockFrequency, const PIO_PIN chipSelectPin, const bool highSensitivity,const uint8_t chargeVoltage, const int8_t tempOffset)
: ChipSelectPin{chipSelectPin}
,configClockFrequency{clockFrequency}
,thresholdFrequency{highSensitivity?thresholdFrequencyDefaultHigh:thresholdFrequencyDefaultLow}
,targetFrequency{highSensitivity?targetFrequencyDefaultHigh:targetFrequencyDefaultLow}
,configSensitivity{highSensitivity?sensitivityDefaultHigh:sensitivityDefaultLow}
,temperatureOffset{tempOffset} {
    LOG_TRACE << "Constructing driver";
    PIO_PinWrite(ChipSelectPin, true);

    LOG_TRACE << "Initializing configuration";
    initConfiguration(chargeVoltage,highSensitivity, false);
    LOG_TRACE << "Done initializing configuration";
}

uint8_t FGDOS::getChipID() const {
    uint8_t readAddress = readMask | 0x13;
    //first value is dont care, second is chipid;
    etl::array<uint8_t, 2> buffer{};
    if (!writeRead(&readAddress, 1, buffer.data(), buffer.size())) {
        LOG_ERROR << "SPI read failed!\n";
        //return error
        return 0;
    }
    return buffer.back();
}

uint32_t FGDOS::getSerialNumber() const {
    uint8_t readAddress = readMask | 0x10;
    //first value is dont care, second is chipid;
    etl::array<uint8_t, 4> buffer{};
    if (!writeRead(&readAddress,1,buffer.data(),buffer.size())) {
        LOG_ERROR << "SPI read failed!\n";
        //return error
        return 0;
    }
    return (static_cast<uint32_t > (buffer[3]) << 16) | (static_cast<uint32_t>(buffer[2]) << 8) | buffer[1];

}

float FGDOS::getTotalDose() {

        calculateDose();
        return previousDose + doseIncrease;

}
float FGDOS::getDoseRate() {
    calculateDose();
    return (doseIncrease * static_cast<float>(configClockFrequency)) / static_cast<float>(windowAmount);
}