/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Declaration of the class CReaderXML.
*
*/




#ifndef XML_READER_H
#define XML_READER_H


// INCLUDE FILES
#include <e32base.h>

#include "cdcreaderbase.h"

// COSTANTS
enum TParamSegment
    {
    EParamName,
    EParamValue
    };
    
class EInputParams
{    
    public:
    
    inline EInputParams( const TInt aId, const TDesC16& aParam ) : 
    iId( aId ),
    iParam( aParam )
    {
    //no implementation required
    };
    
    const TInt         iId;
    const TDesC16&     iParam;    
};


// CLASS DECLARATION

/**
* CReaderXML.
* Reads and parses XML input. Capable of locating and reading Internet, WLAN
* and VPN access points. 
*/
class CReaderXML : public CReaderBase
    {

    public:
        /**
        * Two-phase constructor.
        * @param aInFileName the input file name
        * @param aCharSet the input characterset
        * @return CReaderXML instance
        */
        static CReaderXML* NewLC( const TDesC& aInFileName,
                                  const TDbCreatorInputCharSet aCharSet );
        
        /**
        * Destructor.
        */
        virtual ~CReaderXML();

    protected:
        /**
        * Second phase constructor. Leaves on failure.
        */
        void ConstructL();

        /**
        * Default constructor.
        */
        CReaderXML( const TDbCreatorInputCharSet aCharSet );

    public:
    
        /**
        * Locates the next feature in XML file.
        */
        virtual void LocateFeatureL();
           
        /**
        * Gives back the next line from the given buffer.
        * @return - EFalse if the end of buffer is accessed, 
        *         - ETrue otherwise.
        */
        virtual TBool LocateAPL();
           
        /**
        * Parses a line of the input file.
        */
        virtual void ParseAPL();
        
        /**
        * Resets the actual file position to the beginning of the file.
        * @return One of the system-wide error codes if reset failed.
        */
        virtual TInt Reset();
        
        /**
        * Writes log to the log file.
        */
        virtual void ToDoLog();
    
    private:
        /**
        * Reads one line of the input file.
        * @return - KErrNone if the whole line is read successfully.
        *         - KErrEof if end of file is reached
        *         - KErrNotFound if line delimiter is not found
        *         - KErrGenereal in case of any other error
        */
        TInt ReadLineL();
        
        /**
        * Extracts one line from the buffer read by ReadLineL
        * @param const TPtrC16& aBuffer reference to the buffer descriptor
        * @param TPtrC16& aLine reference to the line descriptor 
        * @return - KErrNone if the whole line is read successfully.
        *         - KErrEof if end of file is reached
        *         - KErrNotFound if line delimiter is not found
        *         - KErrGenereal in case of any other error
        */
        TInt ExtractLine(const TPtrC16& aBuffer, TPtrC16& aLine);
        
        /**
        * Detects the head of feature segment
        * @return the detected feature :
        *          - EFeatureNone  : No feature found
        *          - EFeatureAP    : Internet access point
        *          - EFeatureWLAN  : WLAN access point
        *          - EFeatureVPN   : VPN access point
        */
        TDbCreatorFeatures DetectFeatureHead();
        
        /**
        * Detects the tail of feature segment
        * @return TRUE if the current line contains the tail of feature
        */
        TBool DetectFeatureTail();
        
        /**
        * Detects the tail of access point data segment
        * @return TRUE if the current line contains the tail
        */
        TBool DetectTailAP();
        
        /**
        * Detects the parameter of access point data segment
        * @return TInt the ID the matching field id of the parameter
        */
        TInt DetectParam();
        
        /**
        * Reads the parameter value
        * @param aSegment - the required segment of the param
        *           EParamName - name 
        *           EParamValue - value
        * <ParamName>ParamValue</ParamName>
        * @return pointer to the tag read from the input file. Memory is
        * allocated by the function. NULL can be returned.
        */
        HBufC16* ReadParam( TParamSegment aSegment );
        
        /**
        * Detects the encoding of the xml input.  If detection fails leaves
        * ECharSetAscii as default character set.
        */
        void DetectEncodingL();
        
        
        /**
        * Fills the parameter arrays with the id-name pairs 
        */
        void FillPacketDataParams();
        void FillLanParams();
        void FillWLanParams();
        void FillVpnParams();
        void FillGlobalParams();
        void FillDNParams();
        
        /**
            * Replace special XML chars in connecton method names 
            */
            void ConvertSpecialXMLChars( HBufC* aPtrTag );
            void ReplaceInString( HBufC* aPtrTag, const TDesC& aSpXML, 
                                            const TDesC& aSp );

    private:

        // @var Holds the actual file position
        TInt                    iFilePos;
        
        // @var Input character set
        TDbCreatorInputCharSet  iCharSet;
        
        // @var holds the line
        HBufC16*                iLine;
        
        // @var true if global settings are found
        TBool                   iFoundGlobal;
        
        RArray<EInputParams>    iPDParams;
        RArray<EInputParams>    iLanParams;
        RArray<EInputParams>    iWLanParams;
        RArray<EInputParams>    iVpnParams;
        RArray<EInputParams>    iGlobalParams;
        RArray<EInputParams>    iDNParams;
        
    
    };


#endif // XML_READER_H


// End of File.
