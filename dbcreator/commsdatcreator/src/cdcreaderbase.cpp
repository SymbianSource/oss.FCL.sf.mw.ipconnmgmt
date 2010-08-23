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
* Description:   Implementation of the class CReaderBase
*
*/




// INCLUDE FILES

#include "cdcreaderbase.h"
#include "cdclogger.h"

#include "cdcerrors.h"
#include "cdccommon.h"

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

//---------------------------------------------------------
// Constructor & Destructor
//---------------------------------------------------------
//
CReaderBase::~CReaderBase()
    {
    CLOSE_LOG;
    //no implementation required
    delete iTagContainer;
    
    iInputFile.Close();
    iFs.Close();
    }
    
    
CReaderBase::CReaderBase( TDbCreatorInputFile aFileType,
                          TDbCreatorFeatures aFeature ):
    iCurrentFeature( aFeature ),
    iTagContainer( NULL ),
    iFileType( aFileType )
    {
        //no implementation required
    }

// ---------------------------------------------------------
// CProcessorBase::ConstructL
// ---------------------------------------------------------
//
void CReaderBase::ConstructL( const TDesC& aInFileName )
    {
    User::LeaveIfError( iFs.Connect() );

    TInt error( iInputFile.Open( iFs, aInFileName,
                        EFileShareReadersOnly | EFileStream | EFileRead ) );
                        
    if ( error == KErrNotFound )
        {
        User::Leave( KErrNotFound );
        }
    else if ( error )
        {
        User::Leave( KErrFileCannotBeOpened );
        }
    else
        {
        //nothing to do    
        }
    
    ////iFs.MkDirAll( KFullLogDir );

    CREATE_LOG;

    ////User::LeaveIfError( iLogFile.Connect() );
    ////iLogFile.CreateLog( KLogDir, KLogFile , EFileLoggingModeAppend );
    
       iTagContainer = CTagContainer::NewL();
    }
    
    
// ---------------------------------------------------------
// CProcessorBase::TagContainer
// ---------------------------------------------------------
//
CTagContainer& CReaderBase::TagContainer()
    {
    return *iTagContainer;
    }
    
    
// ---------------------------------------------------------
// CProcessorBase::InputFile
// ---------------------------------------------------------
//
RFile& CReaderBase::InputFile()
    {
    return iInputFile;
    }


// ---------------------------------------------------------
// CProcessorBase::FileLogger
// ---------------------------------------------------------
//
RFileLogger& CReaderBase::FileLogger()
    {
    return iLogFile;    
    }


// ---------------------------------------------------------
// CProcessorBase::FieldIDPresent
// ---------------------------------------------------------
//
TBool CReaderBase::FieldIDPresent()
    {
    return ( iFileType == EFileXML );    
    }

    
// ---------------------------------------------------------
// CProcessorBase::CurrentFeature
// ---------------------------------------------------------
//
TDbCreatorFeatures CReaderBase::CurrentFeature()
    {
    return iCurrentFeature;    
    }
    
//End of file
