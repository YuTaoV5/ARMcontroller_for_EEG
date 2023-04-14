/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../interactions/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[28];
    char stringdata0[355];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 16), // "titleDoubleClick"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 12), // "QMouseEvent*"
QT_MOC_LITERAL(4, 42, 5), // "event"
QT_MOC_LITERAL(5, 48, 20), // "axisLabelDoubleClick"
QT_MOC_LITERAL(6, 69, 8), // "QCPAxis*"
QT_MOC_LITERAL(7, 78, 4), // "axis"
QT_MOC_LITERAL(8, 83, 23), // "QCPAxis::SelectablePart"
QT_MOC_LITERAL(9, 107, 4), // "part"
QT_MOC_LITERAL(10, 112, 17), // "legendDoubleClick"
QT_MOC_LITERAL(11, 130, 10), // "QCPLegend*"
QT_MOC_LITERAL(12, 141, 6), // "legend"
QT_MOC_LITERAL(13, 148, 22), // "QCPAbstractLegendItem*"
QT_MOC_LITERAL(14, 171, 4), // "item"
QT_MOC_LITERAL(15, 176, 16), // "selectionChanged"
QT_MOC_LITERAL(16, 193, 10), // "mousePress"
QT_MOC_LITERAL(17, 204, 10), // "mouseWheel"
QT_MOC_LITERAL(18, 215, 14), // "addRandomGraph"
QT_MOC_LITERAL(19, 230, 19), // "removeSelectedGraph"
QT_MOC_LITERAL(20, 250, 15), // "removeAllGraphs"
QT_MOC_LITERAL(21, 266, 18), // "contextMenuRequest"
QT_MOC_LITERAL(22, 285, 3), // "pos"
QT_MOC_LITERAL(23, 289, 10), // "moveLegend"
QT_MOC_LITERAL(24, 300, 12), // "graphClicked"
QT_MOC_LITERAL(25, 313, 21), // "QCPAbstractPlottable*"
QT_MOC_LITERAL(26, 335, 9), // "plottable"
QT_MOC_LITERAL(27, 345, 9) // "dataIndex"

    },
    "MainWindow\0titleDoubleClick\0\0QMouseEvent*\0"
    "event\0axisLabelDoubleClick\0QCPAxis*\0"
    "axis\0QCPAxis::SelectablePart\0part\0"
    "legendDoubleClick\0QCPLegend*\0legend\0"
    "QCPAbstractLegendItem*\0item\0"
    "selectionChanged\0mousePress\0mouseWheel\0"
    "addRandomGraph\0removeSelectedGraph\0"
    "removeAllGraphs\0contextMenuRequest\0"
    "pos\0moveLegend\0graphClicked\0"
    "QCPAbstractPlottable*\0plottable\0"
    "dataIndex"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   74,    2, 0x08 /* Private */,
       5,    2,   77,    2, 0x08 /* Private */,
      10,    2,   82,    2, 0x08 /* Private */,
      15,    0,   87,    2, 0x08 /* Private */,
      16,    0,   88,    2, 0x08 /* Private */,
      17,    0,   89,    2, 0x08 /* Private */,
      18,    0,   90,    2, 0x08 /* Private */,
      19,    0,   91,    2, 0x08 /* Private */,
      20,    0,   92,    2, 0x08 /* Private */,
      21,    1,   93,    2, 0x08 /* Private */,
      23,    0,   96,    2, 0x08 /* Private */,
      24,    2,   97,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6, 0x80000000 | 8,    7,    9,
    QMetaType::Void, 0x80000000 | 11, 0x80000000 | 13,   12,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,   22,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 25, QMetaType::Int,   26,   27,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->titleDoubleClick((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 1: _t->axisLabelDoubleClick((*reinterpret_cast< QCPAxis*(*)>(_a[1])),(*reinterpret_cast< QCPAxis::SelectablePart(*)>(_a[2]))); break;
        case 2: _t->legendDoubleClick((*reinterpret_cast< QCPLegend*(*)>(_a[1])),(*reinterpret_cast< QCPAbstractLegendItem*(*)>(_a[2]))); break;
        case 3: _t->selectionChanged(); break;
        case 4: _t->mousePress(); break;
        case 5: _t->mouseWheel(); break;
        case 6: _t->addRandomGraph(); break;
        case 7: _t->removeSelectedGraph(); break;
        case 8: _t->removeAllGraphs(); break;
        case 9: _t->contextMenuRequest((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 10: _t->moveLegend(); break;
        case 11: _t->graphClicked((*reinterpret_cast< QCPAbstractPlottable*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QCPAxis* >(); break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QCPAxis::SelectablePart >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QCPAbstractLegendItem* >(); break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QCPLegend* >(); break;
            }
            break;
        case 11:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QCPAbstractPlottable* >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


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
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
