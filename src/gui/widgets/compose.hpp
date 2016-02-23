#ifndef COMPOSE_HPP
#define COMPOSE_HPP

#include "core/WinsockWrapper.h"
#include "core/ExportWrapper.h"

#include <QWidget>

namespace Ui {
class MTCompose;
}

typedef QMap<QString, QString> mapIDName; // ID, display name.

class MTCompose : public QWidget
{
    Q_OBJECT
    
    QString m_subject;            // Message subject line.
    QString m_body;               // Message body. (For forwards, replies, etc.)
    QString m_msgtype;            // Will be "otserver" or "bitmessage" etc.
    QString m_NotaryID;           // If msgtype is "otserver" then the OT Server ID must be set here.

    QString m_senderNymId;        // Sender NymID should always be available.
    int     m_senderMethodId;     // Method #5 might be the sender's Bitmessage connection. Mandatory if msgtype is "bitmessage" (or anything other than otserver.)
    QString m_senderAddress;      // If msgtype is "bitmessage" this will contain a Bitmessage address.

    QString m_recipientNymId;     // If not available, then m_recipientContactID must be available. (For Bitmessage, for example, Nym is optional.)
    int     m_recipientContactId; // Recipient Nym kjsdfds982345 might be Contact #2. (Or Nym itself might be blank, with ONLY Contact!)
    QString m_recipientAddress;   // If msgtype is "bitmessage" this will contain a Bitmessage address.

    bool    m_bSent;

    bool    m_bForwarding = false;
    QString m_forwardSenderNymId;
    QString m_forwardRecipientNymId;
    QString m_forwardSenderAddress;
    QString m_forwardRecipientAddress;
    QString m_forwardSenderName;
    QString m_forwardRecipientName;

public:
    explicit MTCompose(QWidget *parent = 0);
    ~MTCompose();
    
    bool hasSender();              // Determines if a proper sender is available based on msgtype.
    bool hasRecipient();           // Determines if a proper recipient is available based on msgtype.
    bool hasSenderAndRecipient();  // Determines if a proper sender and recipient are available based on msgtype.

    bool sendingThroughOTServer(); // Checks m_msgtype to see if it is set to "otserver".

    void setForwarded() { m_bForwarding = true; }
    void setVariousIds(QString senderNymId, QString recipientNymId, QString senderAddress, QString recipientAddress); // NOTE: These IDs are ONLY used for the "forwarded portion" of the body. They have NOTHING to do with the actual IDs of THIS message being composed.
    void setInitialSubject(QString subject);
    void setInitialBody(QString body);
    void setInitialMsgType(QString msgtype, QString server="");
    void setInitialServer(QString NotaryID);
    void setInitialSenderNym(QString nymId, QString address="");
    void setInitialSenderAddress(QString address);
    void setInitialRecipient(QString nymId, int contactid=0, QString address="");
    void setInitialRecipientNym(QString nymId, QString address="");
    void setInitialRecipientAddress(QString address);
    void setInitialRecipientContactID(int contactid, QString address="");

    bool setRecipientNymBasedOnContact();

    void setSenderNameBasedOnAvailableData();
    void setRecipientNameBasedOnAvailableData();
    void setTransportDisplayBasedOnAvailableData();

    bool chooseSenderAddress   (mapIDName & mapSenderAddresses,    QString qstrMsgTypeDisplay, bool bForce=false);
    bool chooseRecipientAddress(mapIDName & mapRecipientAddresses, QString qstrMsgTypeDisplay);
    bool chooseSenderMethodID  (mapIDName & theMap,                QString qstrMsgTypeDisplay);
    bool chooseRecipientNym    (mapIDName & theMap);
    bool chooseServer          (mapIDName & theMap);

    bool verifySenderAgainstServer   (bool bAsk=true, QString qstrNotaryID=QString("")); // Assumes senderNymId and NotaryID are set.
    bool verifyRecipientAgainstServer(bool bAsk=true, QString qstrNotaryID=QString("")); // Assumes recipientNymId and NotaryID are set.

    void dialog();

    bool sendMessage(QString subject,   QString body, QString fromNymId, QString toNymId, QString fromAddress, QString toAddress,
                     QString viaServer, QString viaTransport, int viaMethodID);

    // This insures that the sender and recipient nym are using the same Msg Method.
    // Which is to say, the same OT server, or both using Bitmessage, etc.
    //
    bool MakeSureCommonMsgMethod();
    bool CheckPotentialCommonMsgMethod(QString qstrMsgTypeAttempt, mapIDName * pmapSenderAddresses=NULL, mapIDName * pmapRecipientAddresses=NULL);
    void FindSenderMsgMethod();     // Recipient has just changed. Does Sender exist? If so, make sure he is compatible with msgtype or find a new one that matches both.
    void FindRecipientMsgMethod();  // Sender has just changed. Does Recipient exist? If so, make sure he is compatible with msgtype or find a new one that matches both.

    QString FindIDMatch(mapIDName map1, mapIDName map2); // Loops through map1 and returns first ID from it found on map2.

signals:
    void balancesChanged();
    void ShowContact(QString);
    void ShowNym(QString);
    void ShowTransport(QString);
    void ShowServer(QString);
    void nymWasJustChecked(QString);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:
    void onBalancesChanged();

    void on_toButton_clicked();
    void on_fromButton_clicked();
    void on_viaButton_clicked();
    void on_sendButton_clicked();
    void on_toolButtonTo_clicked();
    void on_toolButtonFrom_clicked();

    void on_subjectEdit_textChanged(const QString &arg1);

    void on_toolButton_3_clicked();

private:
    bool already_init;

    Ui::MTCompose *ui;
};

#endif // COMPOSE_HPP
