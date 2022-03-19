/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.9)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.9. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[22];
    char stringdata0[298];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 17), // "OnBackLoginDialog"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 15), // "OnServerConnect"
QT_MOC_LITERAL(4, 46, 13), // "OnSocketError"
QT_MOC_LITERAL(5, 60, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(6, 89, 5), // "error"
QT_MOC_LITERAL(7, 95, 15), // "OnServerMsgRecv"
QT_MOC_LITERAL(8, 111, 14), // "OnSndServerMsg"
QT_MOC_LITERAL(9, 126, 7), // "nSystem"
QT_MOC_LITERAL(10, 134, 4), // "nCmd"
QT_MOC_LITERAL(11, 139, 11), // "std::string"
QT_MOC_LITERAL(12, 151, 4), // "data"
QT_MOC_LITERAL(13, 156, 21), // "OnClickTreeWidgetItem"
QT_MOC_LITERAL(14, 178, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(15, 195, 4), // "item"
QT_MOC_LITERAL(16, 200, 20), // "OnCloseTabWidgetSlot"
QT_MOC_LITERAL(17, 221, 6), // "nIndex"
QT_MOC_LITERAL(18, 228, 21), // "OnMenuActionTriggered"
QT_MOC_LITERAL(19, 250, 15), // "On1STimerUpdate"
QT_MOC_LITERAL(20, 266, 25), // "OnRequestModifyServerTime"
QT_MOC_LITERAL(21, 292, 5) // "nTime"

    },
    "MainWindow\0OnBackLoginDialog\0\0"
    "OnServerConnect\0OnSocketError\0"
    "QAbstractSocket::SocketError\0error\0"
    "OnServerMsgRecv\0OnSndServerMsg\0nSystem\0"
    "nCmd\0std::string\0data\0OnClickTreeWidgetItem\0"
    "QTreeWidgetItem*\0item\0OnCloseTabWidgetSlot\0"
    "nIndex\0OnMenuActionTriggered\0"
    "On1STimerUpdate\0OnRequestModifyServerTime\0"
    "nTime"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x0a /* Public */,
       3,    0,   65,    2, 0x0a /* Public */,
       4,    1,   66,    2, 0x0a /* Public */,
       7,    0,   69,    2, 0x0a /* Public */,
       8,    3,   70,    2, 0x0a /* Public */,
      13,    2,   77,    2, 0x0a /* Public */,
      16,    1,   82,    2, 0x0a /* Public */,
      18,    0,   85,    2, 0x0a /* Public */,
      19,    0,   86,    2, 0x0a /* Public */,
      20,    1,   87,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void,
    QMetaType::Void, QMetaType::UShort, QMetaType::UShort, 0x80000000 | 11,    9,   10,   12,
    QMetaType::Void, 0x80000000 | 14, QMetaType::Int,   15,    2,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::ULongLong,   21,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnBackLoginDialog(); break;
        case 1: _t->OnServerConnect(); break;
        case 2: _t->OnSocketError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 3: _t->OnServerMsgRecv(); break;
        case 4: _t->OnSndServerMsg((*reinterpret_cast< quint16(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2])),(*reinterpret_cast< std::string(*)>(_a[3]))); break;
        case 5: _t->OnClickTreeWidgetItem((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: _t->OnCloseTabWidgetSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->OnMenuActionTriggered(); break;
        case 8: _t->On1STimerUpdate(); break;
        case 9: _t->OnRequestModifyServerTime((*reinterpret_cast< quint64(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
