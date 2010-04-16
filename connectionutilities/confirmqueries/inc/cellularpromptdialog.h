/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*
*/


#ifndef __CELLULARPROMPTDIALOG_H__
#define __CELLULARPROMPTDIALOG_H__

#include <QObject>
#include <HbDialog>
#include <hbdevicedialoginterface.h>

class CellularPromptDialog: public HbDialog, public HbDeviceDialogInterface
    {
    Q_OBJECT

    public:
        /* Constructor */
        CellularPromptDialog(const QVariantMap &parameters);
        /* Destructor */
        ~CellularPromptDialog();
        
        /* Function creates the actual dialog widget */
        void createDialog(const QVariantMap &parameters);
        
        /* Device dialog parameters to be set while dialog is displayed.
         * Not supported.
         */
        bool setDeviceDialogParameters(const QVariantMap &parameters);
        
        /* Not supported */
        int deviceDialogError() const;
        
        /* Closes the device dialog */
        void closeDeviceDialog(bool byClient);
        
        /* Returns a pointer to this dialog widget */
        HbPopup *deviceDialogWidget() const;
        
    signals:
        /* Signal is emitted when the dialog is closed */
        void deviceDialogClosed();
    
        /* Data is emitted in QVariantMap when one of the buttons is selected */
        void deviceDialogData(QVariantMap data);
        
    private slots:
        /* Slot that is mapped to the first button's Clicked signal */
        void firstButtonPressed();
        
        /* Slot that is mapped to the second button's Clicked signal */
        void middleButtonPressed();
        
        /* Slot that is mapped to the cancel button's Clicked signal */
        void cancelPressed();
        
        /* Slot that is mapped to the signal that indicates to closing of the dialog */
        void closingDialog();
        
    private:
        Q_DISABLE_COPY(CellularPromptDialog)
   
    private:
        /* Pointer to the confml dialog object */
        HbDialog *mDialog;
        
        /* Tells whether close has already been called for the dialog */
        bool mClose;
    };


#endif // __CELLULARPROMPTDIALOG_H__
