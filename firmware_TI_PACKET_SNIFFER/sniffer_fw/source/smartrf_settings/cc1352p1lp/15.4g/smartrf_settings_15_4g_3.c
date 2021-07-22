//*********************************************************************************
// Generated by SmartRF Studio version 2.13.0 (build #156- CZ settings for CC13x2R and CC26x2R)
// The applied template is compatible with CC13x2 SDK 2.40.xx.xx
// Device: CC1352P Rev. 2.1
//
//*********************************************************************************


//*********************************************************************************
// Parameter summary
// RX Address0: 0xAA 
// RX Address1: 0xBB 
// RX Address Mode: No address check 
// Frequency: 433.00000 MHz
// Data Format: Serial mode disable 
// Deviation: 5.000 kHz
// Packet Length Config: Variable 
// Max Packet Length: 255 
// Packet Length: 20 
// Packet Data: 255 
// Preamble Count: 2 Bytes 
// Preamble Mode: Send 0 as the first preamble bit 
// RX Filter BW: 34.1 kHz
// Symbol Rate: 19.99969 kBaud
// Sync Word: 0x00000000 
// Sync Word Length: 32 Bits 
// TX Power: 14.5 dBm (requires define CCFG_FORCE_VDDR_HH = 1 in ccfg.c, see CC13xx/CC26xx Technical Reference Manual)
// Enable high output power PA: false 
// Whitening: CC1101/CC2500 compatible 

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/rf_mailbox.h)
#include DeviceFamily_constructPath(driverlib/rf_common_cmd.h)
#include DeviceFamily_constructPath(driverlib/rf_prop_cmd.h)
#include <ti/drivers/rf/RF.h>
#include DeviceFamily_constructPath(rf_patches/rf_patch_cpe_multi_protocol.h)
#include "smartrf_settings_15_4g_2.h"


// TI-RTOS RF Mode Object
RF_Mode Ieee154g3_mode =
{
    .rfMode = RF_MODE_AUTO,
    .cpePatchFxn = &rf_patch_cpe_multi_protocol,
    .mcePatchFxn = 0,
    .rfePatchFxn = 0
};


// Overrides for CMD_PROP_RADIO_DIV_SETUP
uint32_t Ieee154g3_pOverrides[] =
{
    // override_phy_simplelink_long_range_dsss2.xml
    // PHY: Configure DSSS SF=2 for payload data
    HW_REG_OVERRIDE(0x5068,0x0100),
    // override_tc440_tc441.xml
    // Tx: Configure PA ramp time, PACTL2.RC=0x3 (in ADI0, set PACTL2[4:3]=0x3)
    ADI_2HALFREG_OVERRIDE(0,16,0x8,0x8,17,0x1,0x1),
    // Rx: Set AGC reference level to 0x1D (default: 0x2E)
    HW_REG_OVERRIDE(0x609C,0x001D),
    // Rx: Set RSSI offset to adjust reported RSSI by -8 dB (default: -2), trimmed for external bias and differential configuration
    (uint32_t)0x000888A3,
    // Rx: Set anti-aliasing filter bandwidth to 0xD (in ADI0, set IFAMPCTL3[7:4]=0xD)
    ADI_HALFREG_OVERRIDE(0,61,0xF,0xD),
    // TX power override
    // DC/DC regulator: In Tx with 14 dBm PA setting, use DCDCCTL5[3:0]=0xF (DITHER_EN=1 and IPEAK=7). In Rx, use default settings.
    (uint32_t)0x00FF88D3,
    // Tx: Set PA trim to max to maximize its output power (in ADI0, set PACTL0=0xF8)
    ADI_REG_OVERRIDE(0,12,0xF8),
    (uint32_t)0xFFFFFFFF
};


// Overrides for CMD_PROP_RADIO_DIV_SETUP_PA
uint32_t Ieee154g3_pOverridesTxStd[] =
{
    // The TX Power element should always be the first in the list
    TX_STD_POWER_OVERRIDE(0x003F),
    // The ANADIV radio parameter based on the LO divider (0) and front-end (0) settings
    (uint32_t)0x11310703,
    // override_phy_tx_pa_ramp_genfsk_std.xml
    // Tx: Configure PA ramping, set wait time before turning off (0x1A ticks of 16/24 us = 17.3 us).
    HW_REG_OVERRIDE(0x6028,0x001A),
    (uint32_t)0xFFFFFFFF
};


// Overrides for CMD_PROP_RADIO_DIV_SETUP_PA
uint32_t Ieee154g3_pOverridesTx20[] =
{
    // The TX Power element should always be the first in the list
    TX20_POWER_OVERRIDE(0x003F00FF),
    // The ANADIV radio parameter based on the LO divider (0) and front-end (0) settings
    (uint32_t)0x11C10703,
    // override_phy_tx_pa_ramp_genfsk_hpa.xml
    // Tx: Configure PA ramping, set wait time before turning off (0x1F ticks of 16/24 us = 20.3 us).
    HW_REG_OVERRIDE(0x6028,0x001F),
    (uint32_t)0xFFFFFFFF
};


// CMD_PROP_RADIO_DIV_SETUP_PA
// Proprietary Mode Radio Setup Command for All Frequency Bands
rfc_CMD_PROP_RADIO_DIV_SETUP_PA_t Ieee154g3_cmdPropRadioDivSetup =
{
    .commandNo = 0x3807,
    .status = 0x0000,
    .pNextOp = 0, // INSERT APPLICABLE POINTER: (uint8_t*)&xxx
    .startTime = 0x00000000,
    .startTrigger.triggerType = 0x0,
    .startTrigger.bEnaCmd = 0x0,
    .startTrigger.triggerNo = 0x0,
    .startTrigger.pastTrig = 0x0,
    .condition.rule = 0x1,
    .condition.nSkip = 0x0,
    .modulation.modType = 0x1,
    .modulation.deviation = 0x14,
    .modulation.deviationStepSz = 0x0,
    .symbolRate.preScale = 0xF,
    .symbolRate.rateWord = 0x3333,
    .symbolRate.decimMode = 0x0,
    .rxBw = 0x4C,
    .preamConf.nPreamBytes = 0x2,
    .preamConf.preamMode = 0x0,
    .formatConf.nSwBits = 0x20,
    .formatConf.bBitReversal = 0x0,
    .formatConf.bMsbFirst = 0x1,
    .formatConf.fecMode = 0x8,
    .formatConf.whitenMode = 0x7,
    .config.frontEndMode = 0x0,
    .config.biasMode = 0x1,
    .config.analogCfgMode = 0x0,
    .config.bNoFsPowerUp = 0x0,
    .txPower = 0x003F,
    .pRegOverride = Ieee154g3_pOverrides,
    .centerFreq = 0x01B1,
    .intFreq = 0x8000,
    .loDivider = 0x0A,
    .pRegOverrideTxStd = Ieee154g3_pOverridesTxStd,
    .pRegOverrideTx20 = Ieee154g3_pOverridesTx20
};


// CMD_FS
// Frequency Synthesizer Programming Command
rfc_CMD_FS_t Ieee154g3_cmdFs =
{
    .commandNo = 0x0803,
    .status = 0x0000,
    .pNextOp = 0, // INSERT APPLICABLE POINTER: (uint8_t*)&xxx
    .startTime = 0x00000000,
    .startTrigger.triggerType = 0x0,
    .startTrigger.bEnaCmd = 0x0,
    .startTrigger.triggerNo = 0x0,
    .startTrigger.pastTrig = 0x0,
    .condition.rule = 0x1,
    .condition.nSkip = 0x0,
    .frequency = 0x01B1,
    .fractFreq = 0x0000,
    .synthConf.bTxMode = 0x0,
    .synthConf.refFreq = 0x0,
    .__dummy0 = 0x00,
    .__dummy1 = 0x00,
    .__dummy2 = 0x00,
    .__dummy3 = 0x0000
};


// CMD_PROP_RX_ADV
// Proprietary Mode Advanced Receive Command
rfc_CMD_PROP_RX_ADV_t Ieee154g3_cmdPropRxAdv =
{
    .commandNo = 0x3804,
    .status = 0x0000,
    .pNextOp = 0, // INSERT APPLICABLE POINTER: (uint8_t*)&xxx
    .startTime = 0x00000000,
    .startTrigger.triggerType = TRIG_NOW,
    .startTrigger.bEnaCmd = 0x0,
    .startTrigger.triggerNo = 0x0,
    .startTrigger.pastTrig = 0x0,
    .condition.rule = 0x1,
    .condition.nSkip = 0x0,
    .pktConf.bFsOff = 0x0,
    .pktConf.bRepeatOk = 0x0,
    .pktConf.bRepeatNok = 0x0,
    .pktConf.bUseCrc = 0x1,
    .pktConf.bCrcIncSw = 0x0,
    .pktConf.bCrcIncHdr = 0x0,
    .pktConf.endType = 0x0,
    .pktConf.filterOp = 0x1,
    .rxConf.bAutoFlushIgnored = 0x0,
    .rxConf.bAutoFlushCrcErr = 0x0,
    .rxConf.bIncludeHdr = 0x1,
    .rxConf.bIncludeCrc = 0x1,
    .rxConf.bAppendRssi = 0x1,
    .rxConf.bAppendTimestamp = 0x0,
    .rxConf.bAppendStatus = 0x1,
    .syncWord0 = 0x00000000,
    .syncWord1 = 0x00000000,
    .maxPktLen = 0x0000,
    .hdrConf.numHdrBits = 16,
    .hdrConf.lenPos = 0,
    .hdrConf.numLenBits = 11,
    .addrConf.addrType = 0,
    .addrConf.addrSize = 0,
    .addrConf.addrPos = 0,
    .addrConf.numAddr = 0,
    .lenOffset = -4,
    .endTrigger.triggerType = TRIG_NEVER,
    .endTrigger.bEnaCmd = 0x0,
    .endTrigger.triggerNo = 0x0,
    .endTrigger.pastTrig = 0x0,
    .endTime = 0x00000000,
    .pAddr = 0, // INSERT APPLICABLE POINTER: (uint8_t*)&xxx
    .pQueue = 0, // INSERT APPLICABLE POINTER: (dataQueue_t*)&xxx
    .pOutput = 0 // INSERT APPLICABLE POINTER: (uint8_t*)&xxx
};


// CMD_PROP_TX_ADV
// Proprietary Mode Advanced Transmit Command
rfc_CMD_PROP_TX_ADV_t Ieee154g3_cmdPropTxAdv =
{
    .commandNo = 0x3803,
    .status = 0x0000,
    .pNextOp = 0, // INSERT APPLICABLE POINTER: (uint8_t*)&xxx
    .startTime = 0x00000000,
    .startTrigger.triggerType = 0x0,
    .startTrigger.bEnaCmd = 0x0,
    .startTrigger.triggerNo = 0x0,
    .startTrigger.pastTrig = 0x0,
    .condition.rule = 0x1,
    .condition.nSkip = 0x0,
    .pktConf.bFsOff = 0x0,
    .pktConf.bUseCrc = 0x1,
    .pktConf.bCrcIncSw = 0x0,
    .pktConf.bCrcIncHdr = 0x0,
    .numHdrBits = 16,
    .pktLen = 0x0000,
    .startConf.bExtTxTrig = 0x0,
    .startConf.inputMode = 0x0,
    .startConf.source = 0x0,
    .preTrigger.triggerType = 0x0,
    .preTrigger.bEnaCmd = 0x0,
    .preTrigger.triggerNo = 0x0,
    .preTrigger.pastTrig = 0x0,
    .preTime = 0x00000000,
    .syncWord = 0x00000000,
    .pPkt = 0, // INSERT APPLICABLE POINTER: (uint8_t*)&xxx
};



