#include "include/frmsearchlanguage.h"
#include "ui_frmsearchlanguage.h"
#include <QMessageBox>

frmSearchLanguage::frmSearchLanguage(Editor *editor, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmSearchLanguage)
{
    ui->setupUi(this);

    setFixedSize(this->width(), this->height());
    setWindowFlags( (windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint);

    QMap<QString, QList<QString> > langs = editor->languages();

    m_languagesModel = new LanguagesModel(langs);
    m_filter = new QSortFilterProxyModel();
    m_filter->setSourceModel(m_languagesModel);
    m_filter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_filter->setFilterKeyColumn(-1);

    ui->tabResults->horizontalHeader()->setStretchLastSection(true);
    ui->tabResults->setModel(m_filter);
    ui->tabResults->resizeColumnsToContents();
    ui->tabResults->resizeRowsToContents();

    ui->txtFilter->setFocus();
}

frmSearchLanguage::LanguagesModel::LanguagesModel(QMap<QString, QList<QString> > languages, QObject *parent) :
    QAbstractTableModel(parent)
{
    foreach (QString lang, languages.keys()) {
        QList<QString> mimes = languages.value(lang);
        foreach (QString mime, mimes) {
            m_languagesList.append(QPair<QString,QString>(lang, mime));
        }
    }
}

frmSearchLanguage::LanguagesModel::~LanguagesModel()
{

}

int frmSearchLanguage::LanguagesModel::rowCount(const QModelIndex &parent) const
{
    return m_languagesList.count();
}

int frmSearchLanguage::LanguagesModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QVariant frmSearchLanguage::LanguagesModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        if (index.column() == 0)
            return m_languagesList.at(index.row()).first;
        else if (index.column() == 1)
            return m_languagesList.at(index.row()).second;
    }
    return QVariant();
}

QVariant frmSearchLanguage::LanguagesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("Category");
            case 1:
                return QString("Mime type");
            }
        }
    }
    return QVariant();
}

QString frmSearchLanguage::selectedLanguage()
{
    QModelIndexList sel = ui->tabResults->selectionModel()->selectedRows(0);
    if (sel.count() > 0) {
        return sel.at(0).data().toString();
    }
    return QString();
}

QString frmSearchLanguage::selectedMimeType()
{
    QModelIndexList sel = ui->tabResults->selectionModel()->selectedRows(1);
    if (sel.count() > 0) {
        return sel.at(0).data().toString();
    }
    return QString();
}

frmSearchLanguage::~frmSearchLanguage()
{
    delete m_filter;
    delete m_languagesModel;
    //delete ui->lstResults;
    delete ui;
}

void frmSearchLanguage::on_txtFilter_textChanged(const QString &arg1)
{
    m_filter->setFilterFixedString(arg1);
    ui->tabResults->selectRow(0);
}

void frmSearchLanguage::on_tabResults_doubleClicked(const QModelIndex &/*index*/)
{
    if (ui->tabResults->selectionModel()->selectedRows().count() > 0)
        this->accept();
}

void frmSearchLanguage::on_buttonBox_accepted()
{
    if (ui->tabResults->selectionModel()->selectedRows().count() > 0)
        this->accept();
    else
        QMessageBox::information(this, tr("No language selected"),
                                 tr("Please select a language to continue."));
}

void frmSearchLanguage::on_txtFilter_returnPressed()
{
    this->on_buttonBox_accepted();
}
