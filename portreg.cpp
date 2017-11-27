#include "mb.h"
#include "mbport.h"

//    MB_ENOREG,                  /*!< illegal register address. */
//    MB_EINVAL,                  /*!< illegal argument. */

eMBErrorCode
readOD( USHORT usIndex, UCHAR * pucRegBuffer, USHORT usNRegs )
{
    // TODO
    return MB_EINVAL;
}

eMBErrorCode
writeOD( USHORT usIndex, UCHAR * pucRegBuffer, USHORT usNRegs )
{
    // TODO
    return MB_EINVAL;
}

// Only accept reading/writing holding registors. The usAddress is OD index of CO_Data, and
// usNRegs must match OD size. subIndexes of an OD entry will be padded to 16-bits.
eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if (eMode == MB_REG_READ) {
        eStatus = readOD(usAddress, pucRegBuffer, usNRegs);
    }
    else if (eMode == MB_REG_WRITE) {
        eStatus = writeOD(usAddress, pucRegBuffer, usNRegs);
    }

    return eStatus;
}

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}

