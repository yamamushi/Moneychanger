#ifndef __STABLE_HPP__
#include <core/stable.hpp>
#endif

#include <gui/widgets/contactdetails.hpp>
#include <ui_contactdetails.h>

#include <gui/widgets/credentials.hpp>
#include <gui/ui/dlgnewcontact.hpp>

#include <gui/widgets/compose.hpp>
#include <gui/widgets/senddlg.hpp>
#include <gui/widgets/dlgchooser.hpp>
#include <gui/widgets/overridecursor.hpp>

#include <core/moneychanger.hpp>
#include <core/handlers/DBHandler.hpp>
#include <core/handlers/contacthandler.hpp>
#include <core/handlers/modelclaims.hpp>
#include <core/mtcomms.h>

#include <opentxs/client/OTAPI.hpp>
#include <opentxs/client/OTAPI_Exec.hpp>
#include <opentxs/client/OpenTransactions.hpp>
#include <opentxs/client/OT_ME.hpp>
#include <opentxs/core/Nym.hpp>
#include <opentxs/core/crypto/OTPasswordData.hpp>

#include <QComboBox>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QMessageBox>
#include <QTreeView>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QTimer>
#include <QDebug>
#include <QRadioButton>
#include <QButtonGroup>

#include <map>
#include <tuple>
#include <string>


MTContactDetails::MTContactDetails(QWidget *parent, MTDetailEdit & theOwner) :
    MTEditDetails(parent, theOwner),
    ui(new Ui::MTContactDetails)
{
    ui->setupUi(this);
    this->setContentsMargins(0, 0, 0, 0);

    ui->lineEditID->setStyleSheet("QLineEdit { background-color: lightgray }");

    // ----------------------------------
    // Note: This is a placekeeper, so later on I can just erase
    // the widget at 0 and replace it with the real header widget.
    //
    m_pHeaderWidget  = new QWidget;
    ui->verticalLayout->insertWidget(0, m_pHeaderWidget);
    // ----------------------------------
    if (!Moneychanger::It()->expertMode())
    {
        ui->lineEditID->setVisible(false);
        ui->labelID->setVisible(false);
    }
}

MTContactDetails::~MTContactDetails()
{
    delete ui;
}


// ----------------------------------
//virtual
int MTContactDetails::GetCustomTabCount()
{
    return (Moneychanger::It()->expertMode()) ? 4 : 2;
}
// ----------------------------------
//virtual
QWidget * MTContactDetails::CreateCustomTab(int nTab)
{
    const int nCustomTabCount = this->GetCustomTabCount();
    // -----------------------------
    if ((nTab < 0) || (nTab >= nCustomTabCount))
        return NULL; // out of bounds.
    // -----------------------------
    QWidget * pReturnValue = NULL;
    // -----------------------------
    switch (nTab)
    {
    case 0: // "Notes" tab
        if (m_pOwner)
        {
            if (m_pPlainTextEditNotes)
            {
                m_pPlainTextEditNotes->setParent(NULL);
                m_pPlainTextEditNotes->disconnect();
                m_pPlainTextEditNotes->deleteLater();

                m_pPlainTextEditNotes = NULL;
            }
            m_pPlainTextEditNotes = new QPlainTextEdit;

            m_pPlainTextEditNotes->setReadOnly(false);
            m_pPlainTextEditNotes->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
            // -------------------------------
            QVBoxLayout * pvBox = new QVBoxLayout;

            QLabel * pLabelContents = new QLabel(tr("Notes (these don't save yet):"));

            pvBox->setAlignment(Qt::AlignTop);
            pvBox->addWidget   (pLabelContents);
            pvBox->addWidget   (m_pPlainTextEditNotes);
            // -------------------------------
            pReturnValue = new QWidget;
            pReturnValue->setContentsMargins(0, 0, 0, 0);
            pReturnValue->setLayout(pvBox);
        }
        break;

    case 1: // "Profile" tab
        if (m_pOwner)
        {
            if (treeWidgetClaims_)
            {
                treeWidgetClaims_->setParent(NULL);
                treeWidgetClaims_->disconnect();
                treeWidgetClaims_->deleteLater();

                treeWidgetClaims_ = NULL;
            }
            treeWidgetClaims_ = new QTreeWidget;

            treeWidgetClaims_->setEditTriggers(QAbstractItemView::NoEditTriggers);
            treeWidgetClaims_->setAlternatingRowColors(true);
            treeWidgetClaims_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
            treeWidgetClaims_->setColumnCount(5);
            // ---------------------------------------
            QStringList labels = {
                  tr("Value")
                , tr("Type")
                , tr("Nym")
                , tr("Primary")
                , tr("Active")
                , tr("Polarity")
            };
            treeWidgetClaims_->setHeaderLabels(labels);
            // -------------------------------
            QVBoxLayout * pvBox = new QVBoxLayout;

            QLabel * pLabel = new QLabel( QString("%1:").arg(tr("Profile")) );

            pvBox->setAlignment(Qt::AlignTop);
            pvBox->addWidget   (pLabel);
            pvBox->addWidget   (treeWidgetClaims_);
            // -------------------------------
            pReturnValue = new QWidget;
            pReturnValue->setContentsMargins(0, 0, 0, 0);
            pReturnValue->setLayout(pvBox);
        }
        break;

    case 2: // "Credentials" tab
        if (m_pOwner)
        {
            if (m_pCredentials)
            {
                m_pCredentials->setParent(NULL);
                m_pCredentials->disconnect();
                m_pCredentials->deleteLater();

                m_pCredentials = NULL;
            }
            m_pCredentials = new MTCredentials(NULL, *m_pOwner);
            pReturnValue = m_pCredentials;
            pReturnValue->setContentsMargins(0, 0, 0, 0);
        }
        break;

    case 3: // "Known IDs" tab
    {
        if (m_pPlainTextEdit)
        {
            m_pPlainTextEdit->setParent(NULL);
            m_pPlainTextEdit->disconnect();
            m_pPlainTextEdit->deleteLater();

            m_pPlainTextEdit = NULL;
        }
        m_pPlainTextEdit = new QPlainTextEdit;

        m_pPlainTextEdit->setReadOnly(true);
        m_pPlainTextEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        // -------------------------------
        QVBoxLayout * pvBox = new QVBoxLayout;

        QLabel * pLabelContents = new QLabel(tr("Known IDs:"));

        pvBox->setAlignment(Qt::AlignTop);
        pvBox->addWidget   (pLabelContents);
        pvBox->addWidget   (m_pPlainTextEdit);
        // -------------------------------
        pReturnValue = new QWidget;
        pReturnValue->setContentsMargins(0, 0, 0, 0);
        pReturnValue->setLayout(pvBox);
    }
        break;

    default:
        qDebug() << QString("Unexpected: MTContactDetails::CreateCustomTab was called with bad index: %1").arg(nTab);
        return NULL;
    }
    // -----------------------------
    return pReturnValue;
}
// ---------------------------------
//virtual
QString  MTContactDetails::GetCustomTabName(int nTab)
{
    const int nCustomTabCount = this->GetCustomTabCount();
    // -----------------------------
    if ((nTab < 0) || (nTab >= nCustomTabCount))
        return QString(""); // out of bounds.
    // -----------------------------
    QString qstrReturnValue("");
    // -----------------------------
    switch (nTab)
    {
    case 0:  qstrReturnValue = "Notes";        break;
    case 1:  qstrReturnValue = "Profile";      break;
    case 2:  qstrReturnValue = "Credentials";  break;
    case 3:  qstrReturnValue = "Known IDs";    break;

    default:
        qDebug() << QString("Unexpected: MTContactDetails::GetCustomTabName was called with bad index: %1").arg(nTab);
        return QString("");
    }
    // -----------------------------
    return qstrReturnValue;
}
// ------------------------------------------------------



//virtual
void MTContactDetails::DeleteButtonClicked()
{
    if (!m_pOwner->m_qstrCurrentID.isEmpty())
    {
        bool bSuccess = MTContactHandler::getInstance()->DeleteContact(m_pOwner->m_qstrCurrentID.toInt());

        if (bSuccess)
        {
            m_pOwner->m_map.remove(m_pOwner->m_qstrCurrentID);
            // ------------------------------------------------
            emit RefreshRecordsAndUpdateMenu();
            // ------------------------------------------------
        }
    }
}

//virtual
void MTContactDetails::AddButtonClicked()
{
    MTDlgNewContact theNewContact(this);
    theNewContact.setWindowTitle(tr("Create New Contact"));
    // -----------------------------------------------
    if (theNewContact.exec() == QDialog::Accepted)
    {
        QString     rawId  = theNewContact.GetId();
        std::string raw_id = rawId.toStdString();
        std::string str_nym_id = raw_id;
        std::string payment_code;

        QString nymID(rawId);
//      QString nymID = theNewContact.GetId();

        if (!nymID.isEmpty())
        {
            if (!opentxs::OTAPI_Wrap::It()->IsValidID(raw_id))
            {
                nymID = "";
                str_nym_id = "";

                const std::string str_temp = opentxs::OTAPI_Wrap::It()->NymIDFromPaymentCode(raw_id);

                if (!str_temp.empty())
                {
                    payment_code = raw_id;
                    str_nym_id = str_temp;
                    nymID = QString::fromStdString(str_nym_id);
                }
            }
            // --------------------------------------------------------
            if (!opentxs::OTAPI_Wrap::It()->IsValidID(str_nym_id))
            {
                QMessageBox::warning(this, tr("Moneychanger"),
                                     tr("Sorry, that is not a valid Open-Transactions Nym ID."));
                return;
            }
            // --------------------------------------------------------
            int nExisting = MTContactHandler::getInstance()->FindContactIDByNymID(nymID);

            if (nExisting > 0)
            {
                QString contactName = MTContactHandler::getInstance()->GetContactName(nExisting);

                QMessageBox::warning(this, tr("Moneychanger"),
                                     tr("Contact '%1' already exists with NymID: %2").arg(contactName).arg(nymID));
                return;
            }
            // -------------------------------------------------------
            //else (a contact doesn't already exist for that NymID)
            //
            int nContact  = MTContactHandler::getInstance()->CreateContactBasedOnNym(nymID);

            if (nContact <= 0)
            {
                QMessageBox::warning(this, tr("Moneychanger"),
                                     tr("Failed trying to create contact for NymID: %1").arg(nymID));
                return;
            }
            // -------------------------------------------------------
            // else (Successfully created the new Contact...)
            // Now let's add this contact to the Map, and refresh the dialog,
            // and then set the new contact as the current one.
            //
            QString qstrContactID = QString("%1").arg(nContact);

            m_pOwner->m_map.insert(qstrContactID, QString("")); // Blank name. (To start.)
            m_pOwner->SetPreSelected(qstrContactID);
            // ------------------------------------------------
            emit RefreshRecordsAndUpdateMenu();
            // ------------------------------------------------
        }
    }
//    else
//    {
//      qDebug() << "MTContactDetails::AddButtonClicked: CANCEL was clicked";
//    }
    // -----------------------------------------------
}


void MTContactDetails::ClearTree()
{
    if (treeWidgetClaims_)
    {
        treeWidgetClaims_->blockSignals(true);
        treeWidgetClaims_->clear();
        treeWidgetClaims_->blockSignals(false);
    }
}


void MTContactDetails::on_pushButtonRefresh_clicked()
{
    if (m_pOwner->m_qstrCurrentID.isEmpty())
        return;
    int nContactId = m_pOwner->m_qstrCurrentID.toInt();
    if (nContactId <= 0)
        return;
    // --------------------------------
    const QString qstrDefaultNymId = Moneychanger::It()->getDefaultNymID();

    if (qstrDefaultNymId.isEmpty())
    {
        QMessageBox::warning(this, tr("Moneychanger"),
                             QString("%1").arg(tr("Your default identity is not set. Please do that and then try again. "
                                                  "(It's needed since an identity must be used to communicate with your "
                                                  "Contact's server in order to download the credentials.)")));
        return;
    }
    const std::string my_nym_id = qstrDefaultNymId.toStdString();
    // --------------------------------
    mapIDName mapNyms, mapServers;

    // TODO Optimize: We COULD just get the NymIDs here, not the map,
    // and thus save the time we waste ascertaining the display Name, which here,
    // we aren't even using.
    //
    const bool bGotNyms = MTContactHandler::getInstance()->GetNyms(mapNyms, nContactId);
    // --------------------------------
    if (!bGotNyms)
    {
        // TODO:
        // If bGotNyms is false, we can still potentially download the related Nym based on
        // the BIP47 payment code, if we have one. That would occur here, and then we could
        // re-try the GetNyms call.

        QMessageBox::warning(this, tr("Moneychanger"),
                             QString("%1").arg(tr("Sorry but there are no Nyms associated with this contact yet. "
                                                  "(This is where we will, in the future, just download those from the DHT.)")));
        return;
    }
    // --------------------------------
    bTimerFired_ = false; //reset it here just in case.
    // --------------------------------
    for (mapIDName::iterator it_nyms = mapNyms.begin(); it_nyms != mapNyms.end(); ++it_nyms)
    {
        QString qstrNymID   = it_nyms.key();
//      QString qstrNymName = it_nyms.value();

        if (qstrNymID.isEmpty()) // Weird, should never happen.
        {
            qDebug() << "MTContactDetails::on_pushButtonRefresh_clicked: Unexpected empty NymId, should not happen. (Returning.)";
            return;
        }
        const std::string str_nym_id = qstrNymID.toStdString();
        // ----------------------------------------------
        const bool bGotServers = MTContactHandler::getInstance()->GetServers(mapServers, qstrNymID);

        if (!bGotServers)
        {
            // Try the default server, then, since there are no known servers for that Nym.
            const QString qstrDefaultNotaryId = Moneychanger::It()->getDefaultNotaryID();

            if (!qstrDefaultNotaryId.isEmpty())
                mapServers.insert(qstrDefaultNotaryId, "(Leaving server name empty since it's unused.)");
        }
        // -------------------------------
        for (mapIDName::iterator it_servers = mapServers.begin(); it_servers != mapServers.end(); ++it_servers)
        {
            QString qstrNotaryID   = it_servers.key();
//          QString qstrNotaryName = it_servers.value();

            if (qstrNotaryID.isEmpty()) // Weird, should never happen.
            {
                qDebug() << "MTContactDetails::on_pushButtonRefresh_clicked: Unexpected empty NotaryID, should not happen. (Returning.)";
                return;
            }
            const std::string notary_id = qstrNotaryID.toStdString();

            // Todo note: May need to verify that I'M registered at this server, before I
            // try to download some guy's credentials from a "server he's known to use."
            // I may not even be registered there, in which case the check_nym call would fail.
            //
            // ------------------------------
            opentxs::OT_ME madeEasy;
            std::string response;
            {
                MTSpinner theSpinner;

                response = madeEasy.check_nym(notary_id, my_nym_id, str_nym_id);
            }

            int32_t nReturnVal = madeEasy.VerifyMessageSuccess(response);

            if (1 == nReturnVal)
            {
                emit nymWasJustChecked(qstrNymID);
                break;
            }
        } // for (servers)
    }
}

void MTContactDetails::onClaimsUpdatedTimer()
{
    bTimerFired_ = false; //reset it here so it'll work again next time.

    // NOTE: By this point, the Nym's claims should be in our local DB.
    // So I'm not worried that we'll repopulate with old claim data
    // (Since the wallet keeps the Nyms cached in RAM) because I won't be
    // populating the GUI from those Nyms anyway, but from tables in the
    // Moneychanger DB.
    //
    emit RefreshRecordsAndUpdateMenu();
}

void MTContactDetails::onClaimsUpdatedForNym(QString nymId)
{
    if (!bTimerFired_)
    {
        bTimerFired_ = true;

        // I'm doing this because perhaps 5 Nyms may update and trigger this slot,
        // all in a row. So I want to make sure the GUI doesn't update 5 times in
        // a row as well! Right now I'm giving it 5 seconds, so presumably it won't
        // take that long (I hope) for all the Nyms to update, before we then go
        // ahead and update the GUI.
        QTimer::singleShot(2000, this, SLOT(onClaimsUpdatedTimer()));
    }
}

void MTContactDetails::RefreshTree(int nContactId, QStringList & qstrlistNymIDs)
{
    if (!treeWidgetClaims_ || (NULL == ui))
        return;
    // ----------------------------------------
    ClearTree();
    // ----------------------------------------
    if ( (0 == nContactId) || (0 == qstrlistNymIDs.size()) )
        return;
    // ----------------------------------------
    treeWidgetClaims_->blockSignals(true);
    // ----------------------------------------
    // Before commencing with the main act, let's iterate all the Nyms once,
    // and construct a map, so we don't end up doing this multiple times below
    // unnecessarily.
    //
    typedef std::pair<std::string, opentxs::OT_API::ClaimSet> NymClaims;
    typedef std::map <std::string, opentxs::OT_API::ClaimSet> mapOfNymClaims;
    typedef std::map <std::string, std::string> mapOfNymNames;

    mapOfNymClaims nym_claims; // Each pair in this map has a NymID and a ClaimSet.
    mapOfNymNames  nym_names;  // Each pair in this map has a NymID and a Nym Name.

    bool bANymWasChecked = false;

    for (int ii = 0; ii < qstrlistNymIDs.size(); ++ii)
    {
        QString qstrNymID = qstrlistNymIDs.at(ii);
        // ---------------------------------------
        if (qstrNymID.isEmpty()) // should never happen.
            continue;
        // ---------------------------------------
        MTNameLookupQT theLookup;
        const std::string str_nym_id   = qstrNymID.toStdString();
        const std::string str_nym_name = theLookup.GetNymName(str_nym_id, "");
        const opentxs::Identifier id_nym(str_nym_id);

        if (!str_nym_id.empty())
        {
            opentxs::Nym * pCurrentNym = opentxs::OTAPI_Wrap::OTAPI()->LoadPublicNym(id_nym, __FUNCTION__);
            std::unique_ptr<opentxs::Nym> pNymAngel(pCurrentNym);

            if (!pNymAngel)
            {
                opentxs::OTPasswordData thePWData("Sometimes need to load private part of nym in order to use its public key. (Fix that!)");

                // Notice we don't set the unique_ptr in this case, since with this function,
                // the wallet already owns the nym and handles all the cleanup automatically.
                //
                pCurrentNym = opentxs::OTAPI_Wrap::OTAPI()->GetOrLoadNym(id_nym,
                                                                         false, //bChecking=false
                                                                         __FUNCTION__,
                                                                         &thePWData);
            }

            // check_nym if not already downloaded.
            if (nullptr == pCurrentNym)
            {
                const QString qstrDefaultNotaryId = Moneychanger::It()->getDefaultNotaryID();
                const QString qstrDefaultNymId    = Moneychanger::It()->getDefaultNymID();

                if (!qstrDefaultNotaryId.isEmpty() && !qstrDefaultNymId.isEmpty())
                {
                    const std::string my_nym_id = qstrDefaultNymId   .toStdString();
                    const std::string notary_id = qstrDefaultNotaryId.toStdString();

                    opentxs::OT_ME madeEasy;
                    std::string response;
                    {
                        MTSpinner theSpinner;

                        response = madeEasy.check_nym(notary_id, my_nym_id, str_nym_id);
                    }

                    int32_t nReturnVal = madeEasy.VerifyMessageSuccess(response);

                    if (1 == nReturnVal)
                    {
                        pCurrentNym = opentxs::OTAPI_Wrap::OTAPI()->LoadPublicNym(id_nym, __FUNCTION__);
                        pNymAngel.reset(pCurrentNym);
                        bANymWasChecked = true;
                        emit nymWasJustChecked(qstrNymID);
                    }
                }
            }

            if ((nullptr != pCurrentNym) && !bANymWasChecked)
            {
                opentxs::OT_API::ClaimSet claims = opentxs::OTAPI_Wrap::OTAPI()->GetClaims(*pCurrentNym);
                // ---------------------------------------
                nym_claims.insert( NymClaims(str_nym_id, claims) );
                nym_names.insert(std::pair<std::string, std::string>(str_nym_id, str_nym_name));
            }
        }
        // ---------------------------------------------
        // Since we're already looping here through all the Nyms known for this
        // contact, let's go ahead and grab all claims from anyone else, that they
        // "have met" that Nym, so later we can display those on the tree in the
        // appropriate place.


        // TODO



    }
    // -------------------------------------------------
    // NOTE: If a nymWasChecked, that means we emitted a signal which the Moneychanger singleton
    // received. That caused it to to upsert any claims from that Nym into the Moneychanger DB,
    // in the claim table.
    //
    // In that same function, Moneychanger then announces by emitting its own signal, that claims
    // were just updated in the database. And that ends up causing the detail edit to refresh itself,
    // which ends up calling RefreshTree() again (the function we're in right now.)
    //
    // So that's why it's okay for us to return here, since we know this function is just going to
    // get called again anyway, and there will be better data available when it does (because by
    // then, it'll have upserted the claims already.)
    //
    if (bANymWasChecked)
        return;
    // -------------------------------------------------
    // This means NONE of the contact's Nyms had any claims.
    // (So there's nothing to put on this tree. Done.)
    if (nym_claims.empty())
        return;
    // -------------------------------------------------
//  QPointer<ModelClaims>      pModelClaims_;
//  QPointer<ClaimsProxyModel> pProxyModelClaims_;

    pProxyModelClaims_ = nullptr;
    pModelClaims_ = DBHandler::getInstance()->getClaimsModel(nContactId);

    if (!pModelClaims_)
        return;

    pProxyModelClaims_ = new ClaimsProxyModel;
    pProxyModelClaims_->setSourceModel(pModelClaims_);
    // -------------------------------------------------
    // We make a button group for each combination of Nym, Section, and Type.
    // Therefore I make a map with a KEY of: tuple<Nym, Section, Type>
    // The VALUE will be pointers to QButtonGroup.
    //
//    const std::string claim_nym_id  = qvarNymId  .isValid() ? qvarNymId.toString().toStdString() : "";
//    const uint32_t    claim_section = qvarSection.isValid() ? qvarSection.toUInt() : 0;
//    const uint32_t    claim_type    = qvarType   .isValid() ? qvarType.toUInt() : 0;

    typedef std::tuple<std::string, uint32_t, uint32_t> ButtonGroupKey;
    typedef std::map<ButtonGroupKey, QButtonGroup *> mapOfButtonGroups;

    mapOfButtonGroups mapButtonGroups;
    // -------------------------------------------------

//    justusranvier
//    6:45 ok
//    6:46 well, when I update opentxs-proto to include the appropriate sections and types, you'll have a Relationships section
//    6:46 and there will be an item type called "met"
//    6:46 if you want to make this claim programmatically
//    6:47 set the indexSection to proto::CONTACTSECTION_RELATIONSHIPS
//    6:47 set the indexSectionType to CITEMTYPE_MET
//    6:47 and set the nymid of the person you met to textValue
//    6:47 (based on that code sample)
//    6:47 .
//    6:48 actually both of those enums should be in the opentxs::proto:: namespace

    // Insert "Has Met" into Tree.
    //
    QString  qstrMetLabel = QString("<b>%1</b>").arg(tr("Relationship claims"));
    QLabel * label = new QLabel(qstrMetLabel, treeWidgetClaims_);

    QTreeWidgetItem * metInPerson = new QTreeWidgetItem;

//  metInPerson->setText(0, "Met in person");
    treeWidgetClaims_->addTopLevelItem(metInPerson);
    treeWidgetClaims_->expandItem(metInPerson);
    treeWidgetClaims_->setItemWidget(metInPerson, 0, label);
    // ------------------------------------------
    // Earlier above, we made a list of all the claims from other Nyms
    // that they "have met" the Nyms represented by this Contact.
    // Now let's add those here as sub-items under the metInPerson top-level item.
    //
    QPointer<ModelClaims> pRelationships = DBHandler::getInstance()->getRelationshipClaims(nContactId);

    if (!pRelationships) // Should never happen, even if the result set is empty.
        return;

    QPointer<ClaimsProxyModel> pProxyModelRelationships = new ClaimsProxyModel;

    pProxyModelRelationships->setSourceModel(pRelationships);

    if (pRelationships->rowCount() > 0)
    {
        // First grab the various relationship type names:  (Parent of, have met, child of, etc.)
        QMap<uint32_t, QString> mapTypeNames;
        // ----------------------------------------
        std::string        sectionName  = opentxs::OTAPI_Wrap::OTAPI()->GetContactSectionName (opentxs::proto::CONTACTSECTION_RELATIONSHIPS);
        std::set<uint32_t> sectionTypes = opentxs::OTAPI_Wrap::OTAPI()->GetContactSectionTypes(opentxs::proto::CONTACTSECTION_RELATIONSHIPS);

        for (auto & indexSectionType: sectionTypes)
        {
            std::string typeName = opentxs::OTAPI_Wrap::OTAPI()->GetContactTypeName(indexSectionType);
            mapTypeNames.insert(indexSectionType, QString::fromStdString(typeName));
        }
        // ---------------------------------------

        for (int nRelationshipCount = 0; nRelationshipCount < pProxyModelRelationships->rowCount(); nRelationshipCount++)
        {
//            QSqlRecord record = pRelationships->record(nRelationshipCount);

            QModelIndex proxyIndexZero        = pProxyModelRelationships->index(nRelationshipCount, 0);
            QModelIndex sourceIndexZero       = pProxyModelRelationships->mapToSource(proxyIndexZero);
            // ----------------------------------------------------------------------------
            QModelIndex sourceIndexClaimId    = pRelationships->sibling(sourceIndexZero.row(), CLAIM_SOURCE_COL_CLAIM_ID,    sourceIndexZero);
            QModelIndex sourceIndexNymId      = pRelationships->sibling(sourceIndexZero.row(), CLAIM_SOURCE_COL_NYM_ID,      sourceIndexZero);
            QModelIndex sourceIndexSection    = pRelationships->sibling(sourceIndexZero.row(), CLAIM_SOURCE_COL_SECTION,     sourceIndexZero);
            QModelIndex sourceIndexType       = pRelationships->sibling(sourceIndexZero.row(), CLAIM_SOURCE_COL_TYPE,        sourceIndexZero);
            QModelIndex sourceIndexValue      = pRelationships->sibling(sourceIndexZero.row(), CLAIM_SOURCE_COL_VALUE,       sourceIndexZero);
            QModelIndex sourceIndexStart      = pRelationships->sibling(sourceIndexZero.row(), CLAIM_SOURCE_COL_START,       sourceIndexZero);
            QModelIndex sourceIndexEnd        = pRelationships->sibling(sourceIndexZero.row(), CLAIM_SOURCE_COL_END,         sourceIndexZero);
            QModelIndex sourceIndexAttributes = pRelationships->sibling(sourceIndexZero.row(), CLAIM_SOURCE_COL_ATTRIBUTES,  sourceIndexZero);
            QModelIndex sourceIndexAttActive  = pRelationships->sibling(sourceIndexZero.row(), CLAIM_SOURCE_COL_ATT_ACTIVE,  sourceIndexZero);
            QModelIndex sourceIndexAttPrimary = pRelationships->sibling(sourceIndexZero.row(), CLAIM_SOURCE_COL_ATT_PRIMARY, sourceIndexZero);
            // ----------------------------------------------------------------------------
            QModelIndex proxyIndexValue       = pProxyModelRelationships->mapFromSource(sourceIndexValue);
            QModelIndex proxyIndexStart       = pProxyModelRelationships->mapFromSource(sourceIndexStart);
            QModelIndex proxyIndexEnd         = pProxyModelRelationships->mapFromSource(sourceIndexEnd);
            // ----------------------------------------------------------------------------
            QVariant    qvarClaimId           = pRelationships->data(sourceIndexClaimId);
            QVariant    qvarNymId             = pRelationships->data(sourceIndexNymId);
            QVariant    qvarSection           = pRelationships->data(sourceIndexSection);
            QVariant    qvarType              = pRelationships->data(sourceIndexType);
            QVariant    qvarValue             = pProxyModelRelationships->data(proxyIndexValue); // Proxy here since the proxy model decodes this. UPDATE: no longer encoded.
            QVariant    qvarStart             = pProxyModelRelationships->data(proxyIndexStart); // Proxy for these two since it formats the
            QVariant    qvarEnd               = pProxyModelRelationships->data(proxyIndexEnd);   // timestamp as a human-readable string.
            QVariant    qvarAttributes        = pRelationships->data(sourceIndexAttributes);
            QVariant    qvarAttActive         = pRelationships->data(sourceIndexAttActive);
            QVariant    qvarAttPrimary        = pRelationships->data(sourceIndexAttPrimary);
            // ----------------------------------------------------------------------------
            const std::string claim_id      = qvarClaimId.isValid() ? qvarClaimId.toString().toStdString() : "";
            const std::string claim_nym_id  = qvarNymId  .isValid() ? qvarNymId.toString().toStdString() : "";
            const uint32_t    claim_section = qvarSection.isValid() ? qvarSection.toUInt() : 0;
            const uint32_t    claim_type    = qvarType   .isValid() ? qvarType.toUInt() : 0;
            const std::string claim_value   = qvarValue  .isValid() ? qvarValue.toString().toStdString() : "";
            // ----------------------------------------------------------------------------
            const bool        claim_active  = qvarAttActive .isValid() ? qvarAttActive .toBool() : false;
            const bool        claim_primary = qvarAttPrimary.isValid() ? qvarAttPrimary.toBool() : false;
            // ----------------------------------------------------------------------------
            QMap<uint32_t, QString>::iterator it_typeNames = mapTypeNames.find(claim_type);
            QString qstrTypeName;

            if (it_typeNames != mapTypeNames.end())
                qstrTypeName = it_typeNames.value();
            // ---------------------------------------
            MTNameLookupQT theLookup;
            const std::string str_claimant_name = theLookup.GetNymName(claim_nym_id, "");

            // Add the claim to the tree.
            //
            QTreeWidgetItem * claim_item = new QTreeWidgetItem;
            // ---------------------------------------

            // ALICE claims she HAS MET *CHARLIE*.

            // str_claimant_name claims she qstrTypeName nym_names[claim_value]

                    //resume
            mapOfNymNames::iterator it_names = nym_names.find(claim_value);
            std::string str_nym_name;

            if (nym_names.end() != it_names)
                str_nym_name =  it_names->second;
            else
                str_nym_name = claim_value;

            const QString qstrClaimantLabel = QString("%1: %2").arg(tr("Claimant")).arg(QString::fromStdString(str_claimant_name));

            claim_item->setText(0, qstrClaimantLabel);      // "Alice" (some lady) from NymId
            claim_item->setText(1, qstrTypeName);           // "Has met" (or so she claimed)
            claim_item->setText(2, QString::fromStdString(str_nym_name));  // "Charlie" (one of the Nyms on this Contact.)

            claim_item->setData(2, Qt::UserRole, QString::fromStdString(claim_nym_id)); // For now this is Alice's Nym Id. Not sure how we'll use it.
            // ----------------------------------------
            // Since this is someone else's claim about me, I should see if I have already confirmed or refuted it.
            bool bPolarity = false;
            const bool bGotPolarity = MTContactHandler::getInstance()->getPolarityIfAny(QString::fromStdString(claim_id),
                                                                                        QString::fromStdString(claim_value), bPolarity);

            if (bGotPolarity)
            {
                claim_item->setText(5, bPolarity ? tr("Confirmed") : tr("Refuted") );
                claim_item->setBackgroundColor(5, bPolarity ? QColor("green") : QColor("red"));
            }
            else
                claim_item->setText(5, tr("No comment"));
            // ---------------------------------------
//          claim_item->setFlags(claim_item->flags() |     Qt::ItemIsEditable);
            claim_item->setFlags(claim_item->flags() & ~ ( Qt::ItemIsEditable | Qt::ItemIsUserCheckable) );
            // ---------------------------------------
            metInPerson->addChild(claim_item);
            treeWidgetClaims_->expandItem(claim_item);
            // ----------------------------------------------------------------------------
            // Couldn't do this until now, when the claim_item has been added to the tree.
            //
//          typedef std::tuple<std::string, uint32_t, uint32_t> ButtonGroupKey;
//          typedef std::map<ButtonGroupKey, QButtonGroup *> mapOfButtonGroups;
//          mapOfButtonGroups mapButtonGroups;

//            ButtonGroupKey keyBtnGroup{claim_nym_id, claim_section, claim_type};
//            mapOfButtonGroups::iterator it_btn_group = mapButtonGroups.find(keyBtnGroup);
//            QButtonGroup * pButtonGroup = nullptr;

//            if (mapButtonGroups.end() != it_btn_group)
//                pButtonGroup = it_btn_group->second;
//            else
//            {
//                // The button group doesn't exist yet, for this tuple.
//                // (So let's create it.)
//                //
//                pButtonGroup = new QButtonGroup(treeWidgetClaims_);
//                mapButtonGroups.insert(std::pair<ButtonGroupKey, QButtonGroup *>(keyBtnGroup, pButtonGroup));
//            }
//            { // "Primary"
//            QRadioButton * pRadioBtn = new QRadioButton(treeWidgetClaims_);
//            pButtonGroup->addButton(pRadioBtn);
//            pRadioBtn->setChecked(claim_primary);
//            pRadioBtn->setEnabled(false);
//            // ---------
//            treeWidgetClaims_->setItemWidget(claim_item, 3, pRadioBtn);
//            }
//            // ----------------------------------------------------------------------------
//            { // "Active"
//            QRadioButton * pRadioBtn = new QRadioButton(treeWidgetClaims_);
//            pRadioBtn->setChecked(claim_active);
//            pRadioBtn->setEnabled(false);
//            // ---------
//            treeWidgetClaims_->setItemWidget(claim_item, 4, pRadioBtn);
//            }

        }

    }



    //resume


    //        QString create_claim_table = "CREATE TABLE IF NOT EXISTS claim"
    //               "(claim_id TEXT PRIMARY KEY,"
    //               " claim_nym_id TEXT,"
    //               " claim_section INTEGER,"
    //               " claim_type INTEGER,"
    //               " claim_value TEXT,"
    //               " claim_start INTEGER,"
    //               " claim_end INTEGER,"
    //               " claim_attributes TEXT,"
    //               " claim_att_active INTEGER,"
    //               " claim_att_primary INTEGER"
    //               ")";

//    // TODO



//    // ------------------------------------------------
//    opentxs::proto::ContactData contactData;
//    contactData.set_version(1); // todo hardcoding.

//    for (auto& it: items) {
//        auto newSection = contactData.add_section();
//        newSection->set_version(1);
//        newSection->set_name(static_cast<opentxs::proto::ContactSectionName>(it.first));

//        for (auto& i: it.second) {
//            auto newItem = newSection->add_item();
//            newItem->set_version(1);
//            newItem->set_type(static_cast<opentxs::proto::ContactItemType>(std::get<0>(i)));
//            newItem->set_value(std::get<1>(i));
//            if (std::get<2>(i)) {
//                newItem->add_attribute(opentxs::proto::CITEMATTR_PRIMARY);
//            }
//            newItem->add_attribute(opentxs::proto::CITEMATTR_ACTIVE);
//        }
//    }
//    // ------------------------------------------------
//    if (!opentxs::OTAPI_Wrap::OTAPI()->SetContactData(*pCurrentNym, contactData))
//        qDebug() << __FUNCTION__ << ": ERROR: Failed trying to Set Contact Data!";
////      else
////          qDebug() << __FUNCTION__ << "SetContactData SUCCESS. items.size(): " << items.size();
//    // ------------------------------------------------




    // ------------------------------------------
    // Now we loop through the sections, and for each, we populate its
    // itemwidgets by looping through the nym_claims we got above.
    //
    std::set<uint32_t> sections = opentxs::OTAPI_Wrap::OTAPI()->GetContactSections();

    for (auto & indexSection: sections)  //Names (for example)
    {
        QMap<uint32_t, QString> mapTypeNames;
        // ----------------------------------------
        std::string        sectionName  = opentxs::OTAPI_Wrap::OTAPI()->GetContactSectionName (indexSection); // Names, Email, URL, etc.
        std::set<uint32_t> sectionTypes = opentxs::OTAPI_Wrap::OTAPI()->GetContactSectionTypes(indexSection); // Business, Personal, etc.

        for (auto & indexSectionType: sectionTypes)
        {
            std::string typeName = opentxs::OTAPI_Wrap::OTAPI()->GetContactTypeName(indexSectionType);
            mapTypeNames.insert(indexSectionType, QString::fromStdString(typeName));
        }
        // ---------------------------------------
        // Insert Section into Tree.
        //
        QTreeWidgetItem * topLevel = new QTreeWidgetItem;
        // ------------------------------------------
        topLevel->setText(0, QString::fromStdString(sectionName));
        // ------------------------------------------
        treeWidgetClaims_->addTopLevelItem(topLevel);
        treeWidgetClaims_->expandItem(topLevel);
        // ------------------------------------------


        // ------------------------------------------
        for (int ii = 0; ii < pProxyModelClaims_->rowCount(); ++ii)
        {
            QModelIndex proxyIndexZero        = pProxyModelClaims_->index(ii, 0);
            QModelIndex sourceIndexZero       = pProxyModelClaims_->mapToSource(proxyIndexZero);
            // ----------------------------------------------------------------------------
            QModelIndex sourceIndexClaimId    = pModelClaims_->sibling(sourceIndexZero.row(), CLAIM_SOURCE_COL_CLAIM_ID,    sourceIndexZero);
            QModelIndex sourceIndexNymId      = pModelClaims_->sibling(sourceIndexZero.row(), CLAIM_SOURCE_COL_NYM_ID,      sourceIndexZero);
            QModelIndex sourceIndexSection    = pModelClaims_->sibling(sourceIndexZero.row(), CLAIM_SOURCE_COL_SECTION,     sourceIndexZero);
            QModelIndex sourceIndexType       = pModelClaims_->sibling(sourceIndexZero.row(), CLAIM_SOURCE_COL_TYPE,        sourceIndexZero);
            QModelIndex sourceIndexValue      = pModelClaims_->sibling(sourceIndexZero.row(), CLAIM_SOURCE_COL_VALUE,       sourceIndexZero);
            QModelIndex sourceIndexStart      = pModelClaims_->sibling(sourceIndexZero.row(), CLAIM_SOURCE_COL_START,       sourceIndexZero);
            QModelIndex sourceIndexEnd        = pModelClaims_->sibling(sourceIndexZero.row(), CLAIM_SOURCE_COL_END,         sourceIndexZero);
            QModelIndex sourceIndexAttributes = pModelClaims_->sibling(sourceIndexZero.row(), CLAIM_SOURCE_COL_ATTRIBUTES,  sourceIndexZero);
            QModelIndex sourceIndexAttActive  = pModelClaims_->sibling(sourceIndexZero.row(), CLAIM_SOURCE_COL_ATT_ACTIVE,  sourceIndexZero);
            QModelIndex sourceIndexAttPrimary = pModelClaims_->sibling(sourceIndexZero.row(), CLAIM_SOURCE_COL_ATT_PRIMARY, sourceIndexZero);
            // ----------------------------------------------------------------------------
            QModelIndex proxyIndexValue       = pProxyModelClaims_->mapFromSource(sourceIndexValue);
            QModelIndex proxyIndexStart       = pProxyModelClaims_->mapFromSource(sourceIndexStart);
            QModelIndex proxyIndexEnd         = pProxyModelClaims_->mapFromSource(sourceIndexEnd);
            // ----------------------------------------------------------------------------
            QVariant    qvarClaimId           = pModelClaims_->data(sourceIndexClaimId);
            QVariant    qvarNymId             = pModelClaims_->data(sourceIndexNymId);
            QVariant    qvarSection           = pModelClaims_->data(sourceIndexSection);
            QVariant    qvarType              = pModelClaims_->data(sourceIndexType);
            QVariant    qvarValue             = pProxyModelClaims_->data(proxyIndexValue); // Proxy here since the proxy model decodes this. UPDATE: no longer encoded.
            QVariant    qvarStart             = pProxyModelClaims_->data(proxyIndexStart); // Proxy for these two since it formats the
            QVariant    qvarEnd               = pProxyModelClaims_->data(proxyIndexEnd);   // timestamp as a human-readable string.
            QVariant    qvarAttributes        = pModelClaims_->data(sourceIndexAttributes);
            QVariant    qvarAttActive         = pModelClaims_->data(sourceIndexAttActive);
            QVariant    qvarAttPrimary        = pModelClaims_->data(sourceIndexAttPrimary);
            // ----------------------------------------------------------------------------
            const std::string claim_nym_id  = qvarNymId  .isValid() ? qvarNymId.toString().toStdString() : "";
            const uint32_t    claim_section = qvarSection.isValid() ? qvarSection.toUInt() : 0;
            const uint32_t    claim_type    = qvarType   .isValid() ? qvarType.toUInt() : 0;
            const std::string claim_value   = qvarValue  .isValid() ? qvarValue.toString().toStdString() : "";
            // ----------------------------------------------------------------------------
            const bool        claim_active  = qvarAttActive .isValid() ? qvarAttActive .toBool() : false;
            const bool        claim_primary = qvarAttPrimary.isValid() ? qvarAttPrimary.toBool() : false;
            // ----------------------------------------------------------------------------
            if (claim_section != indexSection)
                continue;

            QMap<uint32_t, QString>::iterator it_typeNames = mapTypeNames.find(claim_type);
            QString qstrTypeName;

            if (it_typeNames != mapTypeNames.end())
                qstrTypeName = it_typeNames.value();
            // ---------------------------------------
            // Add the claim to the tree.
            //
            QTreeWidgetItem * claim_item = new QTreeWidgetItem;
            // ---------------------------------------
            claim_item->setText(0, QString::fromStdString(claim_value)); // "james@blah.com"
            claim_item->setText(1, qstrTypeName);                        // "Personal"
            claim_item->setText(2, QString::fromStdString(nym_names[claim_nym_id]));
            claim_item->setData(2, Qt::UserRole, QString::fromStdString(claim_nym_id));
            // ---------------------------------------
//          claim_item->setCheckState(3, claim_primary ? Qt::Checked : Qt::Unchecked); // Moved below (as radio button)
//          claim_item->setCheckState(4, claim_active  ? Qt::Checked : Qt::Unchecked); // Moved below (as radio button)
            // ---------------------------------------
            // NOTE: We'll do this for Nyms, not for Contacts.
            // At least, not for claims. (Contacts will be able to edit
            // their own verifications, though.)
            //
//          claim_item->setFlags(claim_item->flags() |     Qt::ItemIsEditable);
            claim_item->setFlags(claim_item->flags() & ~ ( Qt::ItemIsEditable | Qt::ItemIsUserCheckable) );
            // ---------------------------------------
            topLevel->addChild(claim_item);
            treeWidgetClaims_->expandItem(claim_item);
            // ----------------------------------------------------------------------------
            // Couldn't do this until now, when the claim_item has been added to the tree.
            //
//          typedef std::tuple<std::string, uint32_t, uint32_t> ButtonGroupKey;
//          typedef std::map<ButtonGroupKey, QButtonGroup *> mapOfButtonGroups;
//          mapOfButtonGroups mapButtonGroups;

            ButtonGroupKey keyBtnGroup{claim_nym_id, claim_section, claim_type};
            mapOfButtonGroups::iterator it_btn_group = mapButtonGroups.find(keyBtnGroup);
            QButtonGroup * pButtonGroup = nullptr;

            if (mapButtonGroups.end() != it_btn_group)
                pButtonGroup = it_btn_group->second;
            else
            {
                // The button group doesn't exist yet, for this tuple.
                // (So let's create it.)
                //
                pButtonGroup = new QButtonGroup(treeWidgetClaims_);
                mapButtonGroups.insert(std::pair<ButtonGroupKey, QButtonGroup *>(keyBtnGroup, pButtonGroup));
            }
            { // "Primary"
            QRadioButton * pRadioBtn = new QRadioButton(treeWidgetClaims_);
            pButtonGroup->addButton(pRadioBtn);
            pRadioBtn->setChecked(claim_primary);
            pRadioBtn->setEnabled(false);
            // ---------
            treeWidgetClaims_->setItemWidget(claim_item, 3, pRadioBtn);
            }
            // ----------------------------------------------------------------------------
            { // "Active"
            QRadioButton * pRadioBtn = new QRadioButton(treeWidgetClaims_);
            pRadioBtn->setChecked(claim_active);
            pRadioBtn->setEnabled(false);
            // ---------
            treeWidgetClaims_->setItemWidget(claim_item, 4, pRadioBtn);
            }
        }
    }

    treeWidgetClaims_->blockSignals(false);
    treeWidgetClaims_->resizeColumnToContents(0);
    treeWidgetClaims_->resizeColumnToContents(1);
    treeWidgetClaims_->resizeColumnToContents(2);
    treeWidgetClaims_->resizeColumnToContents(3);
    treeWidgetClaims_->resizeColumnToContents(4);
}

void MTContactDetails::ClearContents()
{
    ui->lineEditID  ->setText("");
    ui->lineEditName->setText("");
    // ------------------------------------------
    if (m_pCredentials)
        m_pCredentials->ClearContents();
    // ------------------------------------------
    if (m_pPlainTextEdit)
        m_pPlainTextEdit->setPlainText("");
    // ------------------------------------------
    if (m_pPlainTextEditNotes)
        m_pPlainTextEditNotes->setPlainText("");
    // ------------------------------------------
    ClearTree();
    // ------------------------------------------
    ui->pushButtonMsg->setEnabled(false);
    ui->pushButtonPay->setEnabled(false);
    ui->pushButtonMsg->setProperty("contactid", 0);
    ui->pushButtonPay->setProperty("contactid", 0);
    // ------------------------------------------
    if (m_pAddresses)
    {
        QWidget * pTab = GetTab(2); // Tab 2 is the index (starting at 0) for tab 3. So this means tab 3.

        if (nullptr != pTab)
        {
            QLayout * pLayout = pTab->layout();

            if (nullptr != pLayout)
                pLayout->removeWidget(m_pAddresses);
        }

        m_pAddresses->setParent(NULL);
        m_pAddresses->disconnect();
        m_pAddresses->deleteLater();
        m_pAddresses = NULL;
    }
}


QGroupBox * MTContactDetails::createAddressGroupBox(QString strContactID)
{
    QGroupBox   * pBox = new QGroupBox(tr("P2P Addresses"));
    QVBoxLayout * vbox = new QVBoxLayout;
    // -----------------------------------------------------------------
    // Loop through all known transport methods (communications addresses)
    // known for this Nym,
    mapIDName theMap;

    int nContactID = strContactID.isEmpty() ? 0 : strContactID.toInt();

    if ((nContactID > 0) && MTContactHandler::getInstance()->GetAddressesByContact(theMap, nContactID, QString("")))
    {
        for (mapIDName::iterator it = theMap.begin(); it != theMap.end(); ++it)
        {
            QString qstrID          = it.key();   // QString("%1|%2").arg(qstrType).arg(qstrAddress)
            QString qstrDisplayAddr = it.value(); // QString("%1: %2").arg(qstrTypeDisplay).arg(qstrAddress);

            QStringList stringlist = qstrID.split("|");

            if (stringlist.size() >= 2) // Should always be 2...
            {
                QString qstrType     = stringlist.at(0);
                QString qstrAddress  = stringlist.at(1);
                // --------------------------------------
                std::string strTypeDisplay = MTComms::displayName(qstrType.toStdString());
                QString    qstrTypeDisplay = QString::fromStdString(strTypeDisplay);

                QWidget * pWidget = this->createSingleAddressWidget(nContactID, qstrType, qstrTypeDisplay, qstrAddress);

                if (NULL != pWidget)
                    vbox->addWidget(pWidget);
            }
        }
    }
    // -----------------------------------------------------------------
    QWidget * pWidget = (nContactID > 0) ? this->createNewAddressWidget(nContactID) : NULL;

    if (NULL != pWidget)
        vbox->addWidget(pWidget);
    // -----------------------------------------------------------------
    pBox->setLayout(vbox);

    return pBox;
}



QWidget * MTContactDetails::createSingleAddressWidget(int nContactID, QString qstrType, QString qstrTypeDisplay, QString qstrAddress)
{
    QWidget     * pWidget    = new QWidget;
    QLineEdit   * pType      = new QLineEdit(qstrTypeDisplay);
    QLabel      * pLabel     = new QLabel(tr("Address:"));
//  QLineEdit   * pAddress   = new QLineEdit(qstrDisplayAddr);
    QLineEdit   * pAddress   = new QLineEdit(qstrAddress);
    QPushButton * pBtnDelete = new QPushButton(tr("Delete"));
    // ----------------------------------------------------------
    pType   ->setMinimumWidth(60);
    pLabel  ->setMinimumWidth(55);
    pLabel  ->setMaximumWidth(55);
    pAddress->setMinimumWidth(60);

    pType   ->setReadOnly(true);
    pAddress->setReadOnly(true);

    pType   ->setStyleSheet("QLineEdit { background-color: lightgray }");
    pAddress->setStyleSheet("QLineEdit { background-color: lightgray }");

    pBtnDelete->setProperty("contactid",    nContactID);
    pBtnDelete->setProperty("methodtype",   qstrType);
    pBtnDelete->setProperty("methodaddr",   qstrAddress);
    pBtnDelete->setProperty("methodwidget", VPtr<QWidget>::asQVariant(pWidget));
    // ----------------------------------------------------------
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(pType);
    layout->addWidget(pLabel);
    layout->addWidget(pAddress);
    layout->addWidget(pBtnDelete);
    // ----------------------------------------------------------
    pWidget->setLayout(layout);

    connect(pBtnDelete, SIGNAL(clicked()), this, SLOT(on_btnAddressDelete_clicked()));
    // ----------------------------------------------------------
    layout->setStretch(0,  1);
    layout->setStretch(1, -1);
    layout->setStretch(2,  3);
    layout->setStretch(3,  1);
    // ----------------------------------------------------------
    pType   ->home(false);
    pAddress->home(false);
    // ----------------------------------------------------------
    return pWidget;
}


QWidget * MTContactDetails::createNewAddressWidget(int nContactID)
{
    QWidget     * pWidget = new QWidget;
    QPushButton * pBtnAdd = new QPushButton(tr("Add"));
    /*
    QString create_msg_method = "CREATE TABLE msg_method"
            " (method_id INTEGER PRIMARY KEY,"   // 1, 2, etc.
            "  method_display_name TEXT,"        // "Localhost"
            "  method_type TEXT,"                // "bitmessage"
            "  method_type_display TEXT,"        // "Bitmessage"
            "  method_connect TEXT)";            // "http://username:password@http://127.0.0.1:8332/"
    */
  //QString create_nym_method
  // = "CREATE TABLE nym_method(nym_id TEXT, method_id INTEGER, address TEXT, PRIMARY KEY(nym_id, method_id, address))";
  //QString create_contact_method
  // = "CREATE TABLE contact_method(contact_id INTEGER, method_type TEXT, address TEXT, PRIMARY KEY(contact_id, method_type, address))";

    QComboBox   * pCombo  = new QComboBox;
    mapIDName     mapMethodTypes;
    MTContactHandler::getInstance()->GetMsgMethodTypes(mapMethodTypes);
    // -----------------------------------------------
    int nIndex = -1;
    for (mapIDName::iterator ii = mapMethodTypes.begin(); ii != mapMethodTypes.end(); ++ii)
    {
        ++nIndex; // 0 on first iteration.
        // ------------------------------
        QString method_type         = ii.key();
        QString method_type_display = ii.value();
        // ------------------------------
        pCombo->insertItem(nIndex, method_type_display, method_type);
    }
    // -----------------------------------------------
    if (mapMethodTypes.size() > 0)
        pCombo->setCurrentIndex(0);
    else
        pBtnAdd->setEnabled(false);
    // -----------------------------------------------
    QLabel      * pLabel   = new QLabel(tr("Address:"));
    QLineEdit   * pAddress = new QLineEdit;
    QHBoxLayout * layout   = new QHBoxLayout;
    // -----------------------------------------------
    pCombo   ->setMinimumWidth(60);
    pLabel   ->setMinimumWidth(55);
    pLabel   ->setMaximumWidth(55);
    pAddress ->setMinimumWidth(60);

    pBtnAdd->setProperty("contactid",    nContactID);
    pBtnAdd->setProperty("methodcombo",  VPtr<QWidget>::asQVariant(pCombo));
    pBtnAdd->setProperty("addressedit",  VPtr<QWidget>::asQVariant(pAddress));
    pBtnAdd->setProperty("methodwidget", VPtr<QWidget>::asQVariant(pWidget));
    // -----------------------------------------------
    layout->addWidget(pCombo);
    layout->addWidget(pLabel);
    layout->addWidget(pAddress);
    layout->addWidget(pBtnAdd);
    // -----------------------------------------------
    pWidget->setLayout(layout);
    // -----------------------------------------------
    layout->setStretch(0,  1);
    layout->setStretch(1, -1);
    layout->setStretch(2,  3);
    layout->setStretch(3,  1);
    // -----------------------------------------------
    connect(pBtnAdd, SIGNAL(clicked()), this, SLOT(on_btnAddressAdd_clicked()));
    // -----------------------------------------------
    return pWidget;
}

void MTContactDetails::on_btnAddressAdd_clicked()
{
    QObject * pqobjSender = QObject::sender();

    if (NULL != pqobjSender)
    {
        QPushButton * pBtnAdd = dynamic_cast<QPushButton *>(pqobjSender);

        if (m_pAddresses && (NULL != pBtnAdd))
        {
            QVariant    varContactID   = pBtnAdd->property("contactid");
            QVariant    varMethodCombo = pBtnAdd->property("methodcombo");
            QVariant    varAddressEdit = pBtnAdd->property("addressedit");
            int         nContactID     = varContactID.toInt();
            QComboBox * pCombo         = VPtr<QComboBox>::asPtr(varMethodCombo);
            QLineEdit * pAddressEdit   = VPtr<QLineEdit>::asPtr(varAddressEdit);
            QWidget   * pWidget        = VPtr<QWidget>::asPtr(pBtnAdd->property("methodwidget"));

            if ((nContactID > 0) && (NULL != pCombo) && (NULL != pAddressEdit) && (NULL != pWidget))
            {
                QString qstrMethodType  = QString("");
                QString qstrAddress     = QString("");
                // --------------------------------------------------
                if (pCombo->currentIndex() < 0)
                    return;
                // --------------------------------------------------
                QVariant varMethodType = pCombo->itemData(pCombo->currentIndex());
                qstrMethodType = varMethodType.toString();

                if (qstrMethodType.isEmpty())
                    return;
                // --------------------------------------------------
                qstrAddress = pAddressEdit->text();

                if (qstrAddress.isEmpty())
                    return;
                // --------------------------------------------------
                bool bAdded = MTContactHandler::getInstance()->AddMsgAddressToContact(nContactID, qstrMethodType, qstrAddress);

                if (bAdded) // Let's add it to the GUI, too, then.
                {
                    QString qstrTypeDisplay = pCombo->currentText();
                    // --------------------------------------------------
                    QLayout     * pLayout = m_pAddresses->layout();
                    QVBoxLayout * pVBox   = (NULL == pLayout) ? NULL : dynamic_cast<QVBoxLayout *>(pLayout);

                    if (NULL != pVBox)
                    {
                        QWidget * pNewWidget = this->createSingleAddressWidget(nContactID, qstrMethodType, qstrTypeDisplay, qstrAddress);

                        if (NULL != pNewWidget)
                            pVBox->insertWidget(pVBox->count()-1, pNewWidget);
                    }
                }
            }
        }
    }
}


void MTContactDetails::on_btnAddressDelete_clicked()
{
    QObject * pqobjSender = QObject::sender();

    if (NULL != pqobjSender)
    {
        QPushButton * pBtnDelete = dynamic_cast<QPushButton *>(pqobjSender);

        if (m_pAddresses && (NULL != pBtnDelete))
        {
            QVariant  varContactID   = pBtnDelete->property("contactid");
            QVariant  varMethodType  = pBtnDelete->property("methodtype");
            QVariant  varMethodAddr  = pBtnDelete->property("methodaddr");
            int       nContactID     = varContactID .toInt();
            QString   qstrMethodType = varMethodType.toString();
            QString   qstrAddress    = varMethodAddr.toString();
            QWidget * pWidget        = VPtr<QWidget>::asPtr(pBtnDelete->property("methodwidget"));

            if (NULL != pWidget)
            {
                bool bRemoved = MTContactHandler::getInstance()->RemoveMsgAddressFromContact(nContactID, qstrMethodType, qstrAddress);

                if (bRemoved) // Let's remove it from the GUI, too, then.
                {
                    QLayout * pLayout = m_pAddresses->layout();

                    if (NULL != pLayout)
                    {
                        pLayout->removeWidget(pWidget);

                        pWidget->setParent(NULL);
                        pWidget->disconnect();
                        pWidget->deleteLater();

                        pWidget = NULL;
                    }
                }
            }
        }
    }
}

void MTContactDetails::on_pushButtonPay_clicked()
{
    QVariant varContactID = ui->pushButtonPay->property("contactid");
    int      nContactID   = varContactID.toInt();

    if (nContactID > 0)
    {
        // --------------------------------------------------
        MTSendDlg * send_window = new MTSendDlg(NULL);
        send_window->setAttribute(Qt::WA_DeleteOnClose);
        // --------------------------------------------------
        QString qstrAcct = Moneychanger::It()->get_default_account_id();
        QString qstr_acct_id = qstrAcct.isEmpty() ? QString("") : qstrAcct;

        if (!qstr_acct_id.isEmpty())
            send_window->setInitialMyAcct(qstr_acct_id);
        // ---------------------------------------
        mapIDName theNymMap;

        if (!MTContactHandler::getInstance()->GetNyms(theNymMap, nContactID))
        {
            QMessageBox::warning(this, tr("Moneychanger"), tr("Sorry, there are no NymIDs associated with this contact. Currently only Open-Transactions payments are supported."));
            return;
        }
        else
        {
            QString qstrHisNymId;

            if (theNymMap.size() == 1) // This contact has exactly one Nym, so we'll go with it.
            {
                mapIDName::iterator theNymIt = theNymMap.begin();

                qstrHisNymId = theNymIt.key();
//              QString qstrNymName = theNymIt.value();
            }
            else // There are multiple Nyms to choose from.
            {
                DlgChooser theNymChooser(this);
                theNymChooser.m_map = theNymMap;
                theNymChooser.setWindowTitle(tr("Recipient has multiple Nyms. (Please choose one.)"));
                // -----------------------------------------------
                if (theNymChooser.exec() == QDialog::Accepted)
                    qstrHisNymId = theNymChooser.m_qstrCurrentID;
                else // User must have cancelled.
                    qstrHisNymId = QString("");
            }

            send_window->setInitialHisNym(qstrHisNymId);
        }
        // ---------------------------------------
        send_window->dialog();
        // ---------------------------------------
        Focuser f(send_window);
        f.show();
        f.focus();
    }
}

void MTContactDetails::on_pushButtonMsg_clicked()
{
    QVariant varContactID = ui->pushButtonMsg->property("contactid");
    int      nContactID   = varContactID.toInt();

    //qDebug() << QString("nContactID: %1").arg(nContactID);

    if (nContactID > 0)
    {
        // --------------------------------------------------
        MTCompose * compose_window = new MTCompose;
        compose_window->setAttribute(Qt::WA_DeleteOnClose);
        // --------------------------------------------------
        // If Moneychanger has a default Nym set, we use that for the Sender.
        // (User can always change it.)
        //
        QString qstrDefaultNym = Moneychanger::It()->get_default_nym_id();

        if (!qstrDefaultNym.isEmpty())
            compose_window->setInitialSenderNym(qstrDefaultNym);

        compose_window->setInitialRecipientContactID(nContactID); // We definitely know this, since we're on the Contacts page.
        // --------------------------------------------------
        if (!qstrDefaultNym.isEmpty()) // Sender Nym is set.
        {
            QString qstrTransportType("");
            mapOfCommTypes mapTypes;

            if (MTComms::types(mapTypes) && (mapTypes.size() > 0))
            {
                mapOfCommTypes::iterator it = mapTypes.begin();
                qstrTransportType = QString::fromStdString(it->first);
            } // Likely now qstrTransportType contains "bitmessage"
            // --------------------------------------------------------
            // Do they both support Bitmessage?
            //
            mapIDName mapSenderAddresses, mapRecipientAddresses;

            if (!qstrTransportType.isEmpty() && compose_window->CheckPotentialCommonMsgMethod(qstrTransportType, &mapSenderAddresses, &mapRecipientAddresses))
            {
                compose_window->setInitialMsgType(qstrTransportType);
                // ----------------------------------------------------
                std::string strTypeDisplay = MTComms::displayName(qstrTransportType.toStdString());
                QString    qstrTypeDisplay = QString::fromStdString(strTypeDisplay);

                compose_window->chooseSenderAddress   (mapSenderAddresses,    qstrTypeDisplay, true); //bForce=false by default.
                compose_window->chooseRecipientAddress(mapRecipientAddresses, qstrTypeDisplay);
            }
            // --------------------------------------------------------
            // No? Okay then let's try the default OT server, if one is available.
            else
            {
                QString qstrDefaultServer = Moneychanger::It()->get_default_notary_id();

                if (!qstrDefaultServer.isEmpty() && compose_window->setRecipientNymBasedOnContact() &&
                    compose_window->verifySenderAgainstServer   (false, qstrDefaultServer) &&
                    compose_window->verifyRecipientAgainstServer(false, qstrDefaultServer)) // Notice the false? That's so it doesn't pop up a dialog asking questions.
                    // ---------------------------------------------------
                    compose_window->setInitialServer(qstrDefaultServer);
            }
        }
        // --------------------------------------------------
        compose_window->dialog();

        Focuser f(compose_window);
        f.show();
        f.focus();
        // --------------------------------------------------
        // Recipient has just changed. Does Sender exist? If so, make sure he is
        // compatible with msgtype or find a new one that matches both. We call this
        // here to force the compose dialog to find a matching transport method
        // between sender and recipient, if one isn't already set by now.
        //
        compose_window->FindSenderMsgMethod();
    }
}

static void blah()
{
//resume
//todo

// OpenTransactions.hpp
//EXPORT VerificationSet GetVerificationSet(const Nym& fromNym) const;
// EXPORT bool SetVerifications(Nym& onNym,
//                            const proto::VerificationSet&) const;

// Nym.hpp
//    std::shared_ptr<proto::VerificationSet> VerificationSet() const;
//    bool SetVerificationSet(const proto::VerificationSet& data);

//    proto::Verification Sign(
//        const std::string& claim,
//        const bool polarity,
//        const int64_t start = 0,
//        const int64_t end = 0,
//        const OTPasswordData* pPWData = nullptr) const;
//    bool Verify(const proto::Verification& item) const;

    // VerificationSet has 2 groups, internal and external.
    // Internal is for your signatures on other people's claims.
    // External is for other people's signatures on your claims.
    // When you find that in the external, you copy it to your own credential.
    // So external is for re-publishing other people's verifications of your claims.

    // If we've repudiated any claims, you can add their IDs to the repudiated field in the verification set.
}
//virtual
void MTContactDetails::refresh(QString strID, QString strName)
{
    if ((NULL == ui) || strID.isEmpty())
    {
        ui->pushButtonMsg->setEnabled(false);
        ui->pushButtonPay->setEnabled(false);
        ui->pushButtonMsg->setProperty("contactid", 0);
        ui->pushButtonPay->setProperty("contactid", 0);

        if (m_pPlainTextEdit)
            m_pPlainTextEdit->setPlainText("");

        if (m_pPlainTextEditNotes)
            m_pPlainTextEditNotes->setPlainText("");

        if (treeWidgetClaims_)
            ClearTree();

        return;
    }
    // -----------------------------
    QWidget * pHeaderWidget  = MTEditDetails::CreateDetailHeaderWidget(m_Type, strID, strName, "", "", ":/icons/icons/rolodex_small", false);

    pHeaderWidget->setObjectName(QString("DetailHeader")); // So the stylesheet doesn't get applied to all its sub-widgets.

    if (m_pHeaderWidget)
    {
        ui->verticalLayout->removeWidget(m_pHeaderWidget);

        m_pHeaderWidget->setParent(NULL);
        m_pHeaderWidget->disconnect();
        m_pHeaderWidget->deleteLater();

        m_pHeaderWidget = NULL;
    }
    ui->verticalLayout->insertWidget(0, pHeaderWidget);
    m_pHeaderWidget = pHeaderWidget;
    // ----------------------------------
    ui->lineEditID  ->setText(strID);
    ui->lineEditName->setText(strName);
    // --------------------------------------------
    QLayout   * pLayout    = nullptr;
    QGroupBox * pAddresses = this->createAddressGroupBox(strID);

    QWidget   * pTab = GetTab(2); // Tab 2 is the index (starting at 0) for tab 3. So this means tab 3.

    if (nullptr != pTab)
        pLayout = pTab->layout();

    if (m_pAddresses) // Delete the old one.
    {
        if (nullptr != pLayout)
            pLayout->removeWidget(m_pAddresses);

        m_pAddresses->setParent(NULL);
        m_pAddresses->disconnect();
        m_pAddresses->deleteLater();
        m_pAddresses = NULL;
    }

    if (nullptr != pLayout)
    {
        pLayout->addWidget(pAddresses);
        m_pAddresses = pAddresses;
    }
    else // Should never actually happen.
    {
        delete pAddresses;
        pAddresses = nullptr;
    }
    // ----------------------------------
    QString     strDetails;
    QStringList qstrlistNymIDs;
    // ------------------------------------------
    int nContactID = strID.toInt();
    // ------------------------------------------
    if (nContactID > 0)
    {
        ui->pushButtonMsg->setProperty("contactid", nContactID);
        ui->pushButtonPay->setProperty("contactid", nContactID);
        ui->pushButtonMsg->setEnabled(true);
        ui->pushButtonPay->setEnabled(true);
    }
    else
    {
        ui->pushButtonMsg->setProperty("contactid", 0);
        ui->pushButtonMsg->setEnabled(false);
        ui->pushButtonPay->setProperty("contactid", 0);
        ui->pushButtonPay->setEnabled(false);

        if (m_pPlainTextEdit)
            m_pPlainTextEdit->setPlainText("");

        if (m_pPlainTextEditNotes)
            m_pPlainTextEditNotes->setPlainText("");

        if (treeWidgetClaims_)
            ClearTree();
    }
    // ------------------------------------------
    {
        mapIDName theNymMap;

        if (MTContactHandler::getInstance()->GetNyms(theNymMap, nContactID))
        {
            strDetails += tr("Nyms:\n");

            for (mapIDName::iterator ii = theNymMap.begin(); ii != theNymMap.end(); ii++)
            {
                QString qstrNymID    = ii.key();
                QString qstrNymValue = ii.value();
                // -------------------------------------
                qstrlistNymIDs.append(qstrNymID);
                // -------------------------------------
                strDetails += QString("%1\n").arg(qstrNymID);
                // -------------------------------------
                mapIDName theServerMap;

                if (MTContactHandler::getInstance()->GetServers(theServerMap, qstrNymID))
                {
                    strDetails += tr("Found on servers:\n");

                    for (mapIDName::iterator iii = theServerMap.begin(); iii != theServerMap.end(); iii++)
                    {
                        QString qstrNotaryID    = iii.key();
                        QString qstrServerValue = iii.value();
                        // -------------------------------------
                        strDetails += QString("%1\n").arg(qstrNotaryID);
                        // -------------------------------------
                        mapIDName theAccountMap;

                        if (MTContactHandler::getInstance()->GetAccounts(theAccountMap, qstrNymID, qstrNotaryID, QString("")))
                        {
                            strDetails += tr("Where he owns the accounts:\n");

                            for (mapIDName::iterator iiii = theAccountMap.begin(); iiii != theAccountMap.end(); iiii++)
                            {
                                QString qstrAcctID    = iiii.key();
                                QString qstrAcctValue = iiii.value();
                                // -------------------------------------
                                strDetails += QString("%1\n").arg(qstrAcctID);
                                // -------------------------------------
                            } // for (accounts)
                        } // got accounts
                    } // for (servers)
                    strDetails += QString("\n");
                } // got servers
            } // for (nyms)
            strDetails += QString("\n");
        } // got nyms
    }
    // --------------------------------------------
    // TAB: "Profile"
    //
    RefreshTree(nContactID, qstrlistNymIDs);
    // --------------------------------------------
    // TAB: "Known IDs"
    //
    if (m_pPlainTextEdit)
        m_pPlainTextEdit->setPlainText(strDetails);
    // --------------------------------------------
    // TAB: "Notes"
    //
//    if (m_pPlainTextEditNotes)  //todo
//        m_pPlainTextEditNotes->setPlainText(strDetails);
    // -----------------------------------
    // TAB: "CREDENTIALS"
    //
    if (m_pCredentials)
        m_pCredentials->refresh(qstrlistNymIDs);
    // -----------------------------------------------------------------------
}


// Add a "delete contact" function that the owner dialog can use.
// It will use this to delete the contact from the SQL db:
//
//         bool MTContactHandler::DeleteContact(int nContactID);

void MTContactDetails::on_lineEditName_editingFinished()
{
    int nContactID = m_pOwner->m_qstrCurrentID.toInt();

    if (nContactID > 0)
    {
        bool bSuccess = MTContactHandler::getInstance()->SetContactName(nContactID, ui->lineEditName->text());

        if (bSuccess)
        {
            m_pOwner->m_map.remove(m_pOwner->m_qstrCurrentID);
            m_pOwner->m_map.insert(m_pOwner->m_qstrCurrentID, ui->lineEditName->text());

            m_pOwner->SetPreSelected(m_pOwner->m_qstrCurrentID);
            // ------------------------------------------------
            emit RefreshRecordsAndUpdateMenu();
            // ------------------------------------------------
        }
    }
}


