#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlField>
#include <QDir>
#include <QDebug>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSqlDatabase _db = QSqlDatabase::addDatabase("QSQLITE", "Memo");
    QString dbPath = QDir(QApplication::applicationDirPath()).filePath("Memo.sqlite3");
    _db.setDatabaseName(dbPath);
    qDebug() << dbPath;

    QSqlDatabase db = QSqlDatabase::database("Memo");
    QSqlQuery query(db);
    query.exec(tr("create table Memo(id INTEGER PRIMARY KEY AUTOINCREMENT, Memo TEXT, Created TEXT)"));
    query.exec(tr("insert into Memo(Memo,Created) values('メモ内容だよ', '1999-12-31 23:59:59')"));


    QSqlTableModel model(nullptr, db);
    qDebug() << model.tableName();
    model.setTable("Memo");
    model.setEditStrategy(QSqlTableModel::EditStrategy::OnManualSubmit); // これがないとmodel.removeRow, removeRowsが反映されない
    model.select(); // これがないとquery.exec()で発行したinsert文のデータが残る
    qDebug() << model.tableName();
    qDebug() << "columnCount: " << model.columnCount();

    QSqlRecord rec = model.record();
    for (int i = 0; i < rec.count(); i++) {
        qDebug() << rec.field(i) << ": " << rec.value(i);
    }
    rec.setValue("Memo", QVariant("あああ"));
    rec.setValue("Created", QVariant("1999-12-31 23:59:59"));
    model.insertRecord(0, rec); // 0行目に追加

    QSqlRecord updateRecord = model.record(0); // 0行目のレコードを取得する
    updateRecord.setValue("Memo", QVariant("updateRecordしたよ"));
    model.setRecord(0, updateRecord); // 0行目のレコードを更新する
    qDebug() << model.lastError().text();

//    model.setRecord();
//    model.removeRow(0); // 0行目を削除
//    qDebug() << model.lastError().text();
//    model.removeRows(0, model.rowCount()); // 0行目からcount件を削除
//    qDebug() << model.lastError().text();

    if( model.submitAll() ) {
        model.database().commit();
        qDebug() << "commit()";
    } else {
        model.database().rollback();
        qDebug() << "rollback()";
    }

    query.exec(tr("select * from Memo"));
    while (query.next()) {
        qDebug() << query.value(0) << "|" << query.value(1) << "|" << query.value(2);
    }
//    QSqlRecord record;
//    QSqlField fMemo("Memo");
//    fMemo.setValue("追記したメモ");
//    QSqlField fCreated("Created");
//    fCreated.setValue("1999-12-31 23:59:59");
//    record.append(fMemo);
//    record.append(fCreated);
//    model.insertRecord(0, record);
//    model.submitAll();

    db.close();
    QSqlDatabase::removeDatabase("Memo");
}

MainWindow::~MainWindow()
{
    delete ui;
}
