#ifndef __STABLE_HPP__
#include <core/stable.hpp>
#endif

#include <gui/widgets/pageaddcontract.hpp>
#include <ui_pageaddcontract.h>

#include <QMessageBox>

#include <opentxs/client/OTAPI.hpp>
#include <opentxs/client/OTAPI_Exec.hpp>

MTPageAddContract::MTPageAddContract(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::MTPageAddContract)
{
    ui->setupUi(this);

    this->setCommitPage(true);

    this->registerField("isImporting", ui->radioButton_1);
    this->registerField("isCreating",  ui->radioButton_2);

    this->registerField("contractType",  ui->lineEdit);

    ui->lineEdit->hide();
}


MTPageAddContract::~MTPageAddContract()
{
    delete ui;
}

//virtual
bool MTPageAddContract::validatePage()
{
    QString qstrType(this->field("contractType").toString());

    // If we're creating a server contract, I want to display a warning to the user.
    //
    if ((0 == qstrType.compare("server")) &&
        ui->radioButton_2->isChecked() && // "Create" (versus "Import".)
        (opentxs::OTAPI_Wrap::It()->GetAccountCount() > 0)) {
        QMessageBox::StandardButton reply;

        reply = QMessageBox::question(this, "", QString("%1<br/><br/>%2<br/><br/>%3").arg(tr("Are you sure you want to create a server contract?")).
                                      arg(tr("WARNING: A server contract should be created using a throwaway wallet, which should then be "
                                             "discarded after the contract is created.")).
                                      arg(tr("But the current wallet has accounts in it! (And most likely you do NOT want to discard those accounts.) "
                                             "Therefore you probably do NOT want to proceed with this action. "
                                             "Are you SURE you want to do this? (No, you do not want to do this. Click 'No'.)")),
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No)
            return false;
    }

    return true;
}

//virtual
int MTPageAddContract::nextId() const
{
    // -1 turns it into the last page.
    //
//    if (ui->radioButton_1->isChecked())
//        return -1;

    if (ui->radioButton_1->isChecked())
        return 1; // goes to import page.

    else if (ui->radioButton_2->isChecked())
        return 2; // goes to create page.

    return QWizardPage::nextId();
}

