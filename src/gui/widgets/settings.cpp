#ifndef __STABLE_HPP__
#include <core/stable.hpp>
#endif

#include <gui/widgets/settings.hpp>
#include <ui_settings.h>

#include <core/moneychanger.hpp>
#include <core/translation.hpp>
#include <core/handlers/DBHandler.hpp>
#include <core/handlers/contacthandler.hpp>

#include <opentxs/client/OTAPI.hpp>
#include <opentxs/client/OTAPI_Exec.hpp>
#include <opentxs/client/OpenTransactions.hpp>

#include <opentxs/core/cron/OTCronItem.hpp>
#include <opentxs/core/recurring/OTPaymentPlan.hpp>
#include <opentxs/core/NumList.hpp>
#include <opentxs/core/script/OTSmartContract.hpp>
#include <opentxs/ext/OTPayment.hpp>

#include <QDir>
#include <QMessageBox>
#include <QDebug>
#include <QTableWidgetItem>
#include <QDateTime>

Settings::Settings(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    // ----------------------------------------------
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    language = DBHandler::getInstance()->queryString("SELECT `parameter1` FROM `settings` WHERE `setting`='language'", 0, 0);

    QDir translationDir(TRANSLATIONS_DIRECOTRY);
    QStringList filters;
    filters << "*.qm";
    translationDir.setNameFilters(filters);
    QStringList translationFiles = translationDir.entryList();
    QString translationName;
    ui->comboBoxLanguage->blockSignals(true);
    foreach(translationName, translationFiles)
    {
        translationName.chop(3);
        QLocale nativTranslation(translationName);
        ui->comboBoxLanguage->addItem(nativTranslation.nativeLanguageName(),translationName);
    }
    ui->comboBoxLanguage->setCurrentIndex(ui->comboBoxLanguage->findData(language));
    ui->comboBoxLanguage->blockSignals(false);
#endif
    // *************************************************************
    if (DBHandler::getInstance()->querySize("SELECT `setting`,`parameter1` FROM `settings` WHERE `setting`='expertmode'") <= 0)
    {
        DBHandler::getInstance()->runQuery(QString("INSERT INTO `settings` (`setting`, `parameter1`) VALUES('expertmode','off')"));
        qDebug() << "expertmode setting wasn't set in the database. Setting to 'off'.";
    }
    // ----------------------------------------------
    expertmode_ = MTContactHandler::getInstance()->
            GetValueByID("expertmode", "parameter1", "settings", "setting");

    const bool bExpertMode = (0 == expertmode_.compare("on"));

    ui->checkBoxExpertMode->blockSignals(true);
    if (bExpertMode)
        ui->checkBoxExpertMode->setChecked(true);
    else
        ui->checkBoxExpertMode->setChecked(false);
    ui->checkBoxExpertMode->blockSignals(false);
    // *************************************************************
    if (DBHandler::getInstance()->querySize("SELECT `setting`,`parameter1` FROM `settings` WHERE `setting`='hidenav'") <= 0)
    {
        DBHandler::getInstance()->runQuery(QString("INSERT INTO `settings` (`setting`, `parameter1`) VALUES('hidenav','off')"));
        qDebug() << "hide navigation setting wasn't set in the database. Setting to 'off'.";
    }
    // ----------------------------------------------
    hidenav_ = MTContactHandler::getInstance()->
            GetValueByID("hidenav", "parameter1", "settings", "setting");

    const bool bHideNav = (0 == hidenav_.compare("on"));

    ui->checkBoxHideNav->blockSignals(true);
    if (bHideNav)
        ui->checkBoxHideNav->setChecked(true);
    else
        ui->checkBoxHideNav->setChecked(false);
    ui->checkBoxHideNav->blockSignals(false);
    // ----------------------------------------------




    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->lineEditNymId   ->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->lineEditNotaryId->setContextMenuPolicy(Qt::CustomContextMenu);


    // ----------------------------------------------
    ui->plainTextEditSeed->setPlainText(QString::fromStdString(opentxs::OTAPI_Wrap::OTAPI()->Wallet_GetWords()));
}

Settings::~Settings()
{
    delete ui;
}


/*
    NumList(const std::set<int64_t>& theNumbers);
//  OTNumList(const char* szNumbers); // removed for security reasons.
    NumList(const String& strNumbers);
    NumList(const std::string& strNumbers);
    NumList(int64_t lInput);
    NumList();
    ~NumList();
    bool Add(const String& strNumbers); // if false, means the numbers
                                        // were already there. (At
                                        // least one of them.)
    bool Add(const std::string& strNumbers); // if false, means the
                                             // numbers were already
                                             // there. (At least one of
                                             // them.)
    bool Add(const int64_t& theValue); // if false, means the value was
                                       // already there.
    bool Remove(const int64_t& theValue); // if false, means the value
                                          // was NOT already there.
    bool Verify(const int64_t& theValue) const; // returns true/false
                                                // (whether value is
                                                // already there.)
    bool Add(const NumList& theNumList); // if false, means the numbers
                                         // were already there. (At
                                         // least one of them.)
    bool Add(const std::set<int64_t>& theNumbers); // if false, means the
                                                   // numbers were
                                                   // already there. (At
                                                   // least one of them.)
    bool Remove(const std::set<int64_t>& theNumbers); // if false, means
                                                      // the numbers were
                                                      // NOT already
                                                      // there. (At least
                                                      // one of them.)
    bool Verify(const std::set<int64_t>& theNumbers) const; // True/False, based on whether values are already there. (ALL
                                                            // theNumbers must be present.)
    bool Verify(const NumList& rhs) const; // True/False, based on
                                           // whether OTNumLists MATCH
                                           // in COUNT and CONTENT (NOT
                                           // ORDER.)
    bool VerifyAny(const NumList& rhs) const; // True/False, based on
                                              // whether ANY of rhs are
                                              // found in *this.
    bool VerifyAny(const std::set<int64_t>& setData) const; // Verify
                                                            // whether
                                                            // ANY of the
                                                            // numbers on
                                                            // *this are
                                                            // found in
                                                            // setData.
    int32_t Count() const;
    bool Peek(int64_t& lPeek) const;
    bool Pop();

    // Outputs the numlist as set of numbers. (To iterate OTNumList, call this,
    // then iterate the output.)
    bool Output(std::set<int64_t>& theOutput) const; // returns false if the numlist was empty.

    // Outputs the numlist as a comma-separated string (for serialization, usually.)
    bool Output(String& strOutput) const; // returns false if the
                                          // numlist was empty.
    void Release();
*/

void Settings::on_pushButton_clicked()
{
    class embeddedSetupClass
    {
        QTableWidget * tableWidget_=nullptr;
    public:
        embeddedSetupClass(QTableWidget * tableWidget)
        : tableWidget_(tableWidget) {
            tableWidget_->blockSignals(true);
            while (tableWidget_->rowCount() > 0)
                tableWidget->removeRow(0);
        }
        ~embeddedSetupClass() {
            tableWidget_->blockSignals(false);
            // ---------------------------
            if (tableWidget_->rowCount() > 0)
                tableWidget_->setCurrentCell(0, 0);
        }
    };
    // ---------------------------
    embeddedSetupClass theInstance(ui->tableWidget);
    // ---------------------------
    const QString NymID    = ui->lineEditNymId   ->text();
    const QString NotaryID = ui->lineEditNotaryId->text();
    // ---------------------------
    opentxs::NumList numlistCronIds ( opentxs::OTAPI_Wrap::It()->GetNym_ActiveCronItemIDs(NymID   .toStdString(),
                                                                                          NotaryID.toStdString()) );
    std::set<int64_t> cronIds;
    const bool bIdsArePresent = numlistCronIds.Output(cronIds);

    if (!bIdsArePresent)
        return;
    // ---------------------------
    for (const auto & cronId : cronIds)
    {
        if (0 == cronId) continue;
        // --------------------------------------
        QString qstrCronId     = QString::number(cronId);
        QString qstrDisplayNum = qstrCronId;
        QString qstrAgreementTypeDisplay = tr("Agreement"); // Smart Contract, Recurring Payment, etc.
        QString qstrMyColumn1, qstrMyColumn2;
        // --------------------------------------
        const opentxs::String strCronItem = opentxs::OTAPI_Wrap::It()->GetActiveCronItem(NotaryID.toStdString(), cronId);

        if (!strCronItem.Exists())
            continue;
        // --------------------------------------
        std::unique_ptr<opentxs::OTCronItem> pCronItem ( opentxs::OTCronItem::NewCronItem(strCronItem) );

        if (!pCronItem)
            continue;
        // --------------------------------------
        opentxs::OTPaymentPlan   * pPlan  = dynamic_cast<opentxs::OTPaymentPlan *>  (pCronItem.get());
        opentxs::OTSmartContract * pSmart = dynamic_cast<opentxs::OTSmartContract *>(pCronItem.get());
        // --------------------------------------
        if ( (nullptr != pPlan) || (nullptr != pSmart) )
        {
            opentxs::OTPayment thePayment(strCronItem);

            if (!thePayment.IsValid() || !thePayment.SetTempValues())
                continue;

            int64_t lTransNumDisplay = 0;

            if (!thePayment.GetTransNumDisplay(lTransNumDisplay) || (0 >= lTransNumDisplay) )
                continue; // todo log an error here.

            qstrDisplayNum = QString::number(lTransNumDisplay);

            opentxs::String strMemo;
            if (thePayment.GetMemo(strMemo))
            {
                const std::string str_memo(strMemo.Get());
                qstrMyColumn1 = QString::fromStdString(str_memo);
            }

            if (nullptr != pPlan)
            {
                qstrAgreementTypeDisplay = tr("Recurring Payment");

                const bool      hasInitialPayment            = pPlan->HasInitialPayment();
                const bool      hasPaymentPlan               = pPlan->HasPaymentPlan() ;
                const bool      isInitialPaymentDone         = pPlan->IsInitialPaymentDone();
                // --------------------------------------------------------------------------------
                const time64_t  initialPaymentDate           = pPlan->GetInitialPaymentDate();
                const time64_t  initialPaymentCompletedDate  = pPlan->GetInitialPaymentCompletedDate();
                const time64_t  initialPaymentLastFailedDate = pPlan->GetLastFailedInitialPaymentDate();
                const int32_t   initialPaymentNumFailures    = pPlan->GetNoInitialFailures();
                const int64_t   initialPaymentAmount         = pPlan->GetInitialPaymentAmount();
                // --------------------------------------------------------------------------------
                const int64_t   planAmount                   = pPlan->GetPaymentPlanAmount() ;
                const time64_t  timeBetweenPayments          = pPlan->GetTimeBetweenPayments() ;
                const time64_t  planStartDate                = pPlan->GetPaymentPlanStartDate() ;
                const time64_t  planLength                   = pPlan->GetPaymentPlanLength() ;
                const int32_t   planMaxNumPayments           = pPlan->GetMaximumNoPayments() ;
                const time64_t  planDateOfLastPayment        = pPlan->GetDateOfLastPayment() ;
                const time64_t  planDateOfLastFailedPayment  = pPlan->GetDateOfLastFailedPayment() ;
                const int32_t   planCompletedPaymentCount    = pPlan->GetNoPaymentsDone() ;
                const int32_t   planFailedPaymentCount       = pPlan->GetNoFailedPayments() ;
                // --------------------------------------------------------------------------------
                const opentxs::Identifier idAssetType      = pPlan->GetInstrumentDefinitionID() ;
                const opentxs::String strAssetType(idAssetType);
                const std::string str_asset_type(strAssetType.Get());

                if (hasInitialPayment)
                {
                    QString qstrPreface = tr("initial payment of");
                    // ----------------------------------------------------------------
                    std::string str_formatted;
                    bool bFormatted = false;

                    if ( !str_asset_type.empty() )
                    {
                        str_formatted = opentxs::OTAPI_Wrap::It()->FormatAmount(str_asset_type, initialPaymentAmount);
                        bFormatted = !str_formatted.empty();
                    }
                    // ----------------------------------------
                    QString qstrAmount = bFormatted ? QString::fromStdString(str_formatted) :
                                                      QString::number(initialPaymentAmount);
                    // -----------------------------------------------------
                    QDateTime current_timestamp = QDateTime::currentDateTime();
                    time64_t currentTime = current_timestamp.toTime_t();

                    const bool initialPaymentisDue = (currentTime >= initialPaymentDate);
                    // -----------------------------------------------------
                    QDateTime qDueDate        = QDateTime::fromTime_t(initialPaymentDate);
                    QDateTime qLastFailedDate = QDateTime::fromTime_t(initialPaymentLastFailedDate);
                    QDateTime qCompletedDate  = QDateTime::fromTime_t(initialPaymentCompletedDate);
                    // -----------------------------------------------------
                    QString strDisplayDate(  !initialPaymentisDue  ? qDueDate.toString(Qt::SystemLocaleShortDate) : // If it's not yet due, we show the due date.
                                           (  isInitialPaymentDone ? qCompletedDate.toString(Qt::SystemLocaleShortDate) : // If it IS due, and it's already paid, we show the paid date. Else we show the "last failed" date.
                                                                     ( (0 != initialPaymentLastFailedDate) ? qLastFailedDate.toString(Qt::SystemLocaleShortDate) : tr("never yet attempted")) // If the last failed date exists, show it. Otherwise say 'never yet attempted'.
                                                                     ) );

                    QString qstrStatus(  !initialPaymentisDue  ? tr("due on") : // If it's not yet due, we show the due date.
                                       (  isInitialPaymentDone ? tr("completed on") : // If it IS due, and it's already paid, we show the paid date. Else we show the "last failed" date.
                                                                 ( (0 != initialPaymentLastFailedDate) ? tr("last failed on") : "-")
                                                                 ) );

                    qstrMyColumn2 = QString("%1 %2 %3 %4.").arg(qstrPreface).arg(qstrAmount).arg(qstrStatus).arg(strDisplayDate);
                }
                // --------------------------------------------------------------------------------
                if (hasPaymentPlan)
                {

                }
                // --------------------------------------------------------------------------------
            }
            else if (nullptr != pSmart)
            {
                qstrAgreementTypeDisplay = tr("Smart Contracts");

            }
            else
                continue; // Should never happen.
        }
        else
        {
            // Maybe it's a market trade.
            continue;
        }
        // --------------------------------------
        int column = 0;
        // ----------------------------------
        ui->tableWidget->insertRow(0);
        // ----------------------------------
        ui->tableWidget->setItem(0, column++, new QTableWidgetItem(qstrDisplayNum));
        ui->tableWidget->setItem(0, column++, new QTableWidgetItem(qstrAgreementTypeDisplay));
        ui->tableWidget->setItem(0, column++, new QTableWidgetItem(qstrMyColumn1));
        ui->tableWidget->setItem(0, column++, new QTableWidgetItem(qstrMyColumn2));
        // ----------------------------------
        QTableWidgetItem *pItem = ui->tableWidget->item(0,0);
        if (nullptr != pItem)
           pItem->setData(Qt::UserRole, QVariant(qstrCronId));
    } // for
    // ----------------------------------
}


void Settings::showEvent (QShowEvent * event)
{
    // ----------------------------------------------
//#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
//    ui->comboBoxLanguage->blockSignals(true);
//    ui->comboBoxLanguage->setCurrentIndex(ui->comboBoxLanguage->findData(language));
//    ui->comboBoxLanguage->blockSignals(false);
//#endif
//    // ----------------------------------------------
//    QString qstrExpertMode = MTContactHandler::getInstance()->
//            GetValueByID("expertmode", "parameter1", "settings", "setting");

//    const bool bExpertMode = (0 == qstrExpertMode.compare("on"));

//    ui->checkBoxExpertMode->blockSignals(true);
//    if (bExpertMode)
//        ui->checkBoxExpertMode->setChecked(true);
//    else
//        ui->checkBoxExpertMode->setChecked(false);
//    ui->checkBoxExpertMode->blockSignals(false);
    // ----------------------------------------------
}

void Settings::on_comboBoxLanguage_currentIndexChanged(int index)
{
    ui->pushButtonSave->setEnabled(true);
}

void Settings::on_checkBoxExpertMode_toggled(bool checked)
{
//    QString GetValueByID(QString qstrID, QString column, QString table, QString id_name);
//    bool    SetValueByID(QString qstrID, QString value,  QString column, QString table, QString id_name);
//    QString settings = "CREATE TABLE IF NOT EXISTS settings (setting TEXT PRIMARY KEY, parameter1 TEXT)";

//    expertmode_ = checked ? QString("on") : QString("off");

    ui->pushButtonSave->setEnabled(true);
}

void Settings::on_lineEditNymId_customContextMenuRequested(const QPoint &pos)
{
    QString qstrVal = Moneychanger::It()->get_default_nym_id();

    if (!qstrVal.isEmpty())
        ui->lineEditNymId->setText(qstrVal);
}

void Settings::on_lineEditNotaryId_customContextMenuRequested(const QPoint &pos)
{
    QString qstrVal = Moneychanger::It()->get_default_notary_id();

    if (!qstrVal.isEmpty())
        ui->lineEditNotaryId->setText(qstrVal);
}

void Settings::on_checkBoxHideNav_toggled(bool checked)
{
    ui->pushButtonSave->setEnabled(true);
}

void Settings::on_pushButtonSave_clicked()
{
    bool bExpertModeChanged = false;
    bool bHideNavChanged    = false;
    bool bSettingsChanged   = false;
    // ----------------------------------------------
    QString qstrExpertMode = ui->checkBoxExpertMode->isChecked() ? QString("on") : QString("off");
    QString qstrHideNav    = ui->checkBoxHideNav   ->isChecked() ? QString("on") : QString("off");
    // ----------------------------------------------
    if (0 != expertmode_.compare(qstrExpertMode))
    {
        //bSettingsChanged = true;
        bExpertModeChanged = true;
        expertmode_ = qstrExpertMode;
        MTContactHandler::getInstance()->SetValueByID("expertmode", qstrExpertMode,  "parameter1", "settings", "setting");
    }
    // ----------------------------------------------
    if (0 != hidenav_.compare(qstrHideNav))
    {
        //bSettingsChanged = true;
        bHideNavChanged = true;
        hidenav_ = qstrHideNav;
        MTContactHandler::getInstance()->SetValueByID("hidenav", qstrHideNav,  "parameter1", "settings", "setting");
    }
    // ----------------------------------------------
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

    qDebug() << ui->comboBoxLanguage->itemData(ui->comboBoxLanguage->currentIndex()).toString();
    if (language != ui->comboBoxLanguage->itemData(ui->comboBoxLanguage->currentIndex()).toString())
    {
        bSettingsChanged = true;
        language = ui->comboBoxLanguage->itemData(ui->comboBoxLanguage->currentIndex()).toString();
        DBHandler::getInstance()->runQuery(
                                            QString("UPDATE `settings` SET `setting`='language', `parameter1`= '%1' WHERE `setting`='language'")
                                            .arg(language)
                                          );
    }
#endif
    // ----------------------------------------------
    ui->pushButtonSave->setEnabled(false);

    if (bSettingsChanged)
        QMessageBox::information(this, "Settings saved","The change will take effect after a restart of Moneychanger.");
    // ----------------------------------------------
    if (bExpertModeChanged)
        emit expertModeUpdated(ui->checkBoxExpertMode->isChecked());
    // ----------------------------------------------
    if (bHideNavChanged)
        emit hideNavUpdated(ui->checkBoxHideNav->isChecked());
    // ----------------------------------------------
//  hide();
}


