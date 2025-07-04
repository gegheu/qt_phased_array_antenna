/****************************************************************************
** Meta object code from reading C++ file 'communication.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../communication.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'communication.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_communication_t {
    QByteArrayData data[20];
    char stringdata0[296];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_communication_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_communication_t qt_meta_stringdata_communication = {
    {
QT_MOC_LITERAL(0, 0, 13), // "communication"
QT_MOC_LITERAL(1, 14, 12), // "refreshPorts"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 16), // "handleOpenSerial"
QT_MOC_LITERAL(4, 45, 14), // "handleClearLog"
QT_MOC_LITERAL(5, 60, 17), // "handleClearTxData"
QT_MOC_LITERAL(6, 78, 15), // "handleClearTxRx"
QT_MOC_LITERAL(7, 94, 14), // "handleSendData"
QT_MOC_LITERAL(8, 109, 14), // "handleRecvData"
QT_MOC_LITERAL(9, 124, 13), // "handleSaveINI"
QT_MOC_LITERAL(10, 138, 13), // "handleLoadINI"
QT_MOC_LITERAL(11, 152, 16), // "onConnectClicked"
QT_MOC_LITERAL(12, 169, 13), // "onSendClicked"
QT_MOC_LITERAL(13, 183, 19), // "onDisconnectClicked"
QT_MOC_LITERAL(14, 203, 11), // "onConnected"
QT_MOC_LITERAL(15, 215, 14), // "onDisconnected"
QT_MOC_LITERAL(16, 230, 14), // "onDataReceived"
QT_MOC_LITERAL(17, 245, 15), // "onErrorOccurred"
QT_MOC_LITERAL(18, 261, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(19, 290, 5) // "error"

    },
    "communication\0refreshPorts\0\0"
    "handleOpenSerial\0handleClearLog\0"
    "handleClearTxData\0handleClearTxRx\0"
    "handleSendData\0handleRecvData\0"
    "handleSaveINI\0handleLoadINI\0"
    "onConnectClicked\0onSendClicked\0"
    "onDisconnectClicked\0onConnected\0"
    "onDisconnected\0onDataReceived\0"
    "onErrorOccurred\0QAbstractSocket::SocketError\0"
    "error"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_communication[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x08 /* Private */,
       3,    0,   95,    2, 0x08 /* Private */,
       4,    0,   96,    2, 0x08 /* Private */,
       5,    0,   97,    2, 0x08 /* Private */,
       6,    0,   98,    2, 0x08 /* Private */,
       7,    0,   99,    2, 0x08 /* Private */,
       8,    0,  100,    2, 0x08 /* Private */,
       9,    0,  101,    2, 0x08 /* Private */,
      10,    0,  102,    2, 0x08 /* Private */,
      11,    0,  103,    2, 0x08 /* Private */,
      12,    0,  104,    2, 0x08 /* Private */,
      13,    0,  105,    2, 0x08 /* Private */,
      14,    0,  106,    2, 0x08 /* Private */,
      15,    0,  107,    2, 0x08 /* Private */,
      16,    0,  108,    2, 0x08 /* Private */,
      17,    1,  109,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 18,   19,

       0        // eod
};

void communication::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<communication *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->refreshPorts(); break;
        case 1: _t->handleOpenSerial(); break;
        case 2: _t->handleClearLog(); break;
        case 3: _t->handleClearTxData(); break;
        case 4: _t->handleClearTxRx(); break;
        case 5: _t->handleSendData(); break;
        case 6: _t->handleRecvData(); break;
        case 7: _t->handleSaveINI(); break;
        case 8: _t->handleLoadINI(); break;
        case 9: _t->onConnectClicked(); break;
        case 10: _t->onSendClicked(); break;
        case 11: _t->onDisconnectClicked(); break;
        case 12: _t->onConnected(); break;
        case 13: _t->onDisconnected(); break;
        case 14: _t->onDataReceived(); break;
        case 15: _t->onErrorOccurred((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 15:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject communication::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_communication.data,
    qt_meta_data_communication,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *communication::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *communication::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_communication.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int communication::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
