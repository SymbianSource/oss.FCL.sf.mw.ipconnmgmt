/*
* Copyright (c) 2004,2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Active object wrapper
*
*/



#ifndef C_CAOASYNCWRAPPER_H
#define C_CAOASYNCWRAPPER_H

// INCLUDE FILES
#include <e32base.h>

// LOCAL DEFINITIONS

/**  
 * Panic category for AsyncWrapper
 */
_LIT(  KPanicCategory, "AsyncWrapper" );

/**  
 * Panic reason for AsyncWrapper
 */
const TInt KReason = 0;

// CLASS DEFINITION

/**
 *  Active object wrapper for asynchronous requests.
 *
 *  @lib PDPContextManager2.lib
 *  @since S60 v3.1
 */
template <class T>
NONSHARABLE_CLASS( CAOAsyncWrapper ): public CActive
    {
public:        // Construction

    /**
     * Create a CAOAsyncWrapper object.
     *
     * @since S60 v3.1
     * @param aPt2Object    A ponter to object, for which the following 
     *                      function pointers are called.
     * @param aRequestFpt   A pointer to Request member function.
     * @param aRunLFpt      A pointer to RunL member function.
     * @param aDoCancelFpt  A pointer to DoCancel member function.
     * @param aRunErrorFpt  A pointer to RunError member function. Note:
     *                      aRunErrorFpt is allowed to be NULL, 
                            in contrast to other parameters!
     * @return a pointer to the created instance of CAOAsyncWrapper.
     */
    static CAOAsyncWrapper* NewL( T* aPt2Object,
                              void ( T::*aRequestFpt )( TRequestStatus& ),
                           void ( T::*aRunLFpt )( TInt ),
                           void ( T::*aDoCancelFpt )(),
                           TInt ( T::*aRunErrorFpt )( TInt ) )
        {
        CAOAsyncWrapper* self = NewLC( aPt2Object, aRequestFpt, 
                                   aRunLFpt, aDoCancelFpt, aRunErrorFpt );
        CleanupStack::Pop(self);
        return self;
        };
    
    /**
     * Create a CAOAsyncWrapper object.
     * Created instance is put on the CleanupStack.
     *
     * @since S60 v3.1
     * @param aPt2Object    A ponter to object, for which the following 
     *                      function pointers are called.
     * @param aRequestFpt   A pointer to Request member function.
     * @param aRunLFpt      A pointer to RunL member function.
     * @param aDoCancelFpt  A pointer to DoCancel member function.
     * @param aRunErrorFpt  A pointer to RunError member function. Note:
     *                      aRunErrorFpt is allowed to be NULL, 
     *                      in contrast to other parameters!
     * @return a pointer to the created instance of CAOAsyncWrapper.
     */
    static CAOAsyncWrapper* NewLC( T* aPt2Object,
                              void ( T::*aRequestFpt )( TRequestStatus& ),
                            void ( T::*aRunLFpt )( TInt ),
                            void ( T::*aDoCancelFpt )(),
                            TInt ( T::*aRunErrorFpt )( TInt ) )
        {
        CAOAsyncWrapper* self = new ( ELeave ) CAOAsyncWrapper( aPt2Object,
                                                                aRequestFpt, 
                                                                aRunLFpt,
                                                                aDoCancelFpt,
                                                               aRunErrorFpt );
        CleanupStack::PushL( self );
        self->ConstructL();
        return self;
        };

    /**
     * Destructor. Destroy the object and release all memory objects.
     * Active object is also canceled.
     *
     * @since S60 v3.1
     */
    virtual ~CAOAsyncWrapper()
        {
        Cancel();
        };


public:        // New methods

    /**
     * Issue asynchronous request.
     * Calls in construction given "request" function pointer, if 
     * AO is not already active.
     * Note: IssueRequestL may leave only if iRequestFpt may leave!
     *
     * @since S60 v3.1
     */
    void IssueRequestL()
        {
        if ( !IsActive() )
            {
            (*iPt2Object.*iRequestFpt)( iStatus );
            SetActive();
            }
        };

    /**
     * Issue asynchronous request.
     * Calls in construction given "request" function pointer, if 
     * AO is not already active.
     * Note: Non-leavable method, should be used only if 
     * issue request method is also non-leavable.
     *
     * @since S60 v3.1
     */
    void IssueRequest()
        {
        if ( !IsActive() )
            {
            (*iPt2Object.*iRequestFpt)( iStatus );
            SetActive();
            }
        };


private:    // Construction

    /**
     * Constructor
     *
     * @since S60 v3.1
     * @param aPt2Object    A ponter to object, for which the following 
     *                      function pointers are called.
     * @param aRequestFpt   A pointer to Request member function.
     * @param aRunLFpt      A pointer to RunL member function.
     * @param aDoCancelFpt  A pointer to DoCancel member function.
     * @param aRunErrorFpt  A pointer to RunError member function. Note:
     *                      aRunErrorFpt is allowed to be NULL, 
     *                      in contrast to other parameters!
     */
    CAOAsyncWrapper( T* aPt2Object,
              void ( T::*aRequestFpt )( TRequestStatus& ),
              void ( T::*aRunLFpt )( TInt ),
              void ( T::*aDoCancelFpt )(),
              TInt ( T::*aRunErrorFpt )( TInt ) )  :
    CActive( CActive::EPriorityStandard ),
    iPt2Object( aPt2Object ),
    iRequestFpt( aRequestFpt ),
    iRunLFpt( aRunLFpt ), 
    iDoCancelFpt( aDoCancelFpt ),
    iRunErrorFpt( aRunErrorFpt )
        {
        __ASSERT_ALWAYS( iPt2Object, User::Panic( KPanicCategory, KReason ) );
        __ASSERT_ALWAYS( iRequestFpt,User::Panic( KPanicCategory, KReason ) );
        __ASSERT_ALWAYS( iRunLFpt,   User::Panic( KPanicCategory, KReason ) );
        __ASSERT_ALWAYS( iDoCancelFpt,
                                     User::Panic( KPanicCategory, KReason ) );
        CActiveScheduler::Add( this );        
        };
                
                
    /**
     * Performs second phase construction of this object
     *
     * @since S60 v3.1
     */
    void ConstructL()
        {
        // Empty
        };


private:    

// from CActive

    /**
     * From CActive
     * Cancel outstanding request
     *
     * @since S60 v3.1
     */
    void DoCancel()
        {
        (*iPt2Object.*iDoCancelFpt)();
        };

    /**
     * From CActive
     * Process the completed request.
     *
     * @since S60 v3.1
     */
    void RunL()
        {
        (*iPt2Object.*iRunLFpt)( iStatus.Int() );
        };

    /**
     * From CActive
     * Handle a leave occurred in the RunL().
     * Note: If RunError function pointer is not given in construction,
     * the leave error is to be propagated back to the active scheduler!
     *
     * @since S60 v3.1
     * @param aError the error code of leave
     */
    TInt RunError( TInt aError )
        {
        if ( iRunErrorFpt )
            {
            return (*iPt2Object.*iRunErrorFpt)( aError );
            }
        else
            {
            return aError;
            }
        }


private:    // Member variables

    /**
     * ?description_of_pointer_member
     * Not own.
     */
    T* iPt2Object;

    /**
     * ?description_of_pointer_member
     * Not own.
     */
    void ( T::*iRequestFpt )( TRequestStatus& );

    /**
     * ?description_of_pointer_member
     * Not own.
     */
    void ( T::*iRunLFpt )( TInt );

    /**
     * ?description_of_pointer_member
     * Not own.
     */
    void ( T::*iDoCancelFpt )();

    /**
     * ?description_of_pointer_member
     * Not own.
     */
    TInt ( T::*iRunErrorFpt )( TInt );    // May be NULL

    };

#endif // C_CAOASYNCWRAPPER_H
