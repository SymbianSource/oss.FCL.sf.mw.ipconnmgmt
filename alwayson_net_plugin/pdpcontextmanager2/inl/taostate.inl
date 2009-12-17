/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Inline function implementations for TAOState 
*
*/


// ---------------------------------------------------------------------------
// Setting a flag
// ---------------------------------------------------------------------------
//
inline void TAOState::SetFlag( TUint aFlag )
    {
    iFlags |= aFlag;
    }

// ---------------------------------------------------------------------------
// Clears the flag
// ---------------------------------------------------------------------------
//
inline void TAOState::ClearFlag( TUint aFlag )
    {
    iFlags &= ~aFlag;
    }

// ---------------------------------------------------------------------------
// Checks if flag is set
// ---------------------------------------------------------------------------
//
inline TBool TAOState::CheckFlag( TUint aFlag )
    {
    return iFlags & aFlag;
    }
    
// ---------------------------------------------------------------------------
// Clears all flags
// ---------------------------------------------------------------------------
//
inline void TAOState::ClearFlags()
    {
    iFlags = 0;
    }

