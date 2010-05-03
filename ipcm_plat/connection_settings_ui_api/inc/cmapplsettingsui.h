/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* CM Manager Application Settings UI interface.  
*
*/

#ifndef CMAPPLSETTINGSUI_H
#define CMAPPLSETTINGSUI_H

// System includes

#include <QObject>
#include <QFlags>
#include <QSet>

// User includes

// Forward declarations

class CmApplSettingsUiPrivate;
class TestCmApplSettingsUi;

// External data types

// Constants

#ifdef BUILD_CMAPPLSETTINGSUI_DLL
#define CMAPPLSETTINGSUIDLL_EXPORT Q_DECL_EXPORT
#else
#define CMAPPLSETTINGSUIDLL_EXPORT Q_DECL_IMPORT
#endif

// Class declaration

/*!
    \class CmApplSettingsUi
    \brief Class for launching the Application Settings UI.

    Displays a radio button list dialog for an application that can be used
    to select a destination or connection method.

*/

class CMAPPLSETTINGSUIDLL_EXPORT CmApplSettingsUi : public QObject
{
    Q_OBJECT
    
public:

    // Data types

    /*! 
       The selection type of a list item in application settings UI
     */
    enum SelectionType
        {
        //! The user selected a destination
        SelectionTypeDestination,
        //! The user selected a connection method
        SelectionTypeConnectionMethod
        };

    /*! 
       These flags control what items the selection UI can contain.
       Flags that are not listed in the enum are reserved for future usage
       and they are disabled by default. So extending these flags later on
       does not cause a BC break. These flags are used as bit fields.
       
       The following enumerations control how destinations and 
       Connection Methods are presented in the selection UI.
       
       ShowDestinations is set and ShowConnectionMethods is set:
         Destination list is shown in selection UI and alternatively
         the user can select a Connection Method.
       ShowDestinations is set and ShowConnectionMethods is not set:
         Destination list is shown in selection UI and no specific
         Connection Method can be selected. 
       ShowDestinations is not set and ShowConnectionMethods is set:
         Connection Method list is shown in selection UI. Destinations
         can not be selected.
       ShowDestinations is not set and ShowConnectionMethods is not set:
         The selection UI will not contain any Destination or
         Connection Method. If no extension(s) are implemented  (later
         on), no dialog is shown, and the execution is cancelled
         (ApplSettingsErrorNoContent).
     */
    enum SelectionDialogItems
        {
        //! Support destination selection
        ShowDestinations = 0x01,
        //! Support connection method selection
        ShowConnectionMethods = 0x02                              
        };
    
    /*!
       Error codes for the dialog.
     */
    enum ApplSettingsError {
        //! No error. Dialog succeeded.
        ApplSettingsErrorNone = 0,
        //! Selection was canceled.
        ApplSettingsErrorCancel,
        //! No items to show (e.g. too strict filters).
        ApplSettingsErrorNoContent,
        //! Settings reading failed.
        ApplSettingsErrorSettingsRead
    };

    /*!
       Details of a setting selection. Used both for configuring and
       selection result.
     */
    struct SettingSelection
        {
        /*!
           Selection type, refers to the type of the list item which
           is selected.
         */
        SelectionType result;
        
        /*!
           Id of the selected destination or connection method.
         */
        uint id;
        };
    
    /*!
       Typedef for the bearer filter parameter.
       This type is used to filter Connection Methods by bearer type.

       The Bearer types are defined in CM Manager's Connection Settings
       Shim (cmmanagerdefines_shim.h). 
     */
    typedef uint BearerTypeFilter;
    
    /*!
        Constructor.
        
        @param parent Parent object.
     */
    explicit CmApplSettingsUi(QObject *parent = 0);
    
    /*!
        Destructor.
     */
    virtual ~CmApplSettingsUi();

    /*!
        Set dialog options. If this function is not called before running the
        dialog, the default options are used:
        -listItems: ShowDestinations | ShowConnectionMethods
        -bearerFilter: empty

        @param[in] listItems Dialog list items configuration.
        @param[in] bearerFilter Dialog Connection Method bearer type filter.
        If empty, all bearers are supported.
     */
    void setOptions(
        const QFlags<SelectionDialogItems> &listItems,
        const QSet<BearerTypeFilter> &bearerFilter);

    /*!
        Dialog selection setter. Sets the current selection in the dialog.
        If not selected, or if the selection is invalid, the default
        selection is used (the first item in the dialog items list).

        @param[in] selection Dialog selection to set.
     */
    void setSelection(const SettingSelection &selection);

    /*!
        Dialog selection getter. Returns the current selection.

        @return Current dialog selection.
     */
    SettingSelection selection() const;

    /*!
        Launches the Application Settings UI provided for applications
        for selecting Destinations and/or Connection Methods.

        If a list of Destinations is displayed, and the selection is set to
        a Connection Method, the destination list highlight is set to
        "Dedicated access point".

        This function runs the dialog asynchronously.  After the dialog
        has been closed (or cancelled), the signal finished() is emmitted.
        If the dialog succeeded, the client can then check the new
        selection with selection() getter.
     */
    void open();

signals:

    /*!
       Signal used for informing that the dialog has finished.
       
       @param[in] status Dialog run status code (ApplSettingsError*).
     */
    void finished(uint status);

public slots:

protected:

protected slots:

private:

    Q_DISABLE_COPY(CmApplSettingsUi)

private slots:

private: // data

    CmApplSettingsUiPrivate *d_ptr;         //!< Private implementation

    // Friend classes
    
    // This is defined as a friend class in order to be able to emit
    // signals directly from private implementation code.
    friend class CmApplSettingsUiPrivate;
    
    // Test class needs direct access
    friend class TestCmApplSettingsUi;
};

#endif // CMAPPLSETTINGSUI_H
