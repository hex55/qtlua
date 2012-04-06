#include "lua_qtextedit.h"

static setter_map<QLabel> lqlabel_set_map;
static setter_map<QTextEdit> lqtextedit_set_map;
static setter_map<QLineEdit> lqlineedit_set_map;

SIGNAL_PROPERYT(lqtextedit, textChanged, QTextEdit, "()")

QLabel* lqlabel_init(QLabel* widget, const object& obj)
{
    lqwidget_init(widget,obj);
    return lq_general_init(widget, obj, lqlabel_set_map);
}

QTextEdit* lqtextedit_init(QTextEdit* widget, const object& obj)
{
    lqwidget_init(widget,obj);
    //lq_general_init(widget, obj, lqtextedit_set_map);
    if( type(obj) == LUA_TTABLE){
        for (iterator i(obj), e; i != e; ++i){
            if(type(i.key()) == LUA_TSTRING){
                QString key = object_cast<QString>(i.key());
                if(lqtextedit_set_map.find(key) != lqtextedit_set_map.end()){
                    lqtextedit_set_map[key](widget,*i);
                }else if(key.compare("textChanged", Qt::CaseInsensitive) == 0){
                    lqtextedit_set_textChanged(widget, *i);
                }
            }
        }
    }
    return widget;
}

QLineEdit* lqlineedit_init(QLineEdit* widget, const object& obj)
{
    lqwidget_init(widget,obj);
    return lq_general_init(widget, obj, lqlineedit_set_map);
}

template<>
void table_init_general<QLabel>(const luabind::argument & arg, const object& obj)
{
    lqlabel_init(construct<QLabel>(arg), obj);
}

template<>
void table_init_general<QTextEdit>(const luabind::argument & arg, const object& obj)
{
    lqtextedit_init(construct<QTextEdit>(arg), obj);
}

template<>
void table_init_general<QLineEdit>(const luabind::argument & arg, const object& obj)
{
    lqlineedit_init(construct<QLineEdit>(arg), obj);
}

int lqlabel_textFormat(QLabel* l)
{
    return l->textFormat();
}
void lqlabel_setTextFormat(QLabel* l, int format)
{
    l->setTextFormat(Qt::TextFormat(format));
}

LQLabel lqlabel()
{
    return
    myclass_<QLabel,QFrame>("QLabel",lqlabel_set_map)
    .def(constructor<>())
    .def(constructor<const QString&>())
    .def(constructor<const QString&,QWidget*>())
    .def("__call", lqlabel_init)
    .def("__init", table_init_general<QLabel>)

    .property("text", &QLabel::text, &QLabel::setText)
    .property("indent", &QLabel::indent, &QLabel::setIndent)
    .property("margin", &QLabel::margin, &QLabel::setMargin)
    .property("textFormat", lqlabel_textFormat, lqlabel_setTextFormat)
    .property("scaledContents", &QLabel::hasScaledContents, &QLabel::setScaledContents)
    .property("wordWrap", &QLabel::wordWrap, &QLabel::setWordWrap)

    .class_<QLabel,QFrame>::property("buddy", &QLabel::buddy, &QLabel::setBuddy)
    ;
}

int lqtextedit_v_scroll_bar(QTextEdit* edit)
{
    return edit->verticalScrollBarPolicy();
}

void lqtextedit_set_v_scroll_bar(QTextEdit* edit, int p)
{
    return edit->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy(p));
}

int lqtextedit_h_scroll_bar(QTextEdit* edit)
{
    return edit->horizontalScrollBarPolicy();
}

void lqtextedit_set_h_scroll_bar(QTextEdit* edit, int p)
{
    return edit->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy(p));
}

int lqtextedit_lineWrapMode(QTextEdit* edit)
{
    return edit->lineWrapMode();
}

void lqtextedit_set_lineWrapMode(QTextEdit* edit, int p)
{
    return edit->setLineWrapMode(QTextEdit::LineWrapMode(p));
}

LQTextEdit lqtextedit()
{
    return
    myclass_<QTextEdit,QAbstractScrollArea>("QTextEdit",lqtextedit_set_map)
    .def(constructor<>())
    .def(constructor<const QString&>())
    .def(constructor<const QString&,QWidget*>())
    .def("__call", lqtextedit_init)
    .def("__init", table_init_general<QTextEdit>)
    .def("clear", &QTextEdit::clear)
    .def("append", &QTextEdit::append)

    .property("text", &QTextEdit::toPlainText, &QTextEdit::setPlainText)
    .property("plainText", &QTextEdit::toPlainText, &QTextEdit::setPlainText)
    .property("html", &QTextEdit::toHtml, &QTextEdit::setHtml)
    .property("readOnly", &QTextEdit::isReadOnly, &QTextEdit::setReadOnly)
    .property("verticalScrollBar", lqtextedit_v_scroll_bar, lqtextedit_set_v_scroll_bar)
    .property("horizontalScrollBar", lqtextedit_h_scroll_bar, lqtextedit_set_h_scroll_bar)
    .property("vScrollBar", lqtextedit_v_scroll_bar, lqtextedit_set_v_scroll_bar)
    .property("hScrollBar", lqtextedit_h_scroll_bar, lqtextedit_set_h_scroll_bar)
    .property("lineWrapMode", lqtextedit_lineWrapMode, lqtextedit_set_lineWrapMode)
    .class_<QTextEdit,QAbstractScrollArea>::property("textChanged", lqtextedit_get_textChanged, lqtextedit_set_textChanged)
    ;
}

LQLineEdit lqlineedit()
{
    return
    myclass_<QLineEdit,QWidget>("QLineEdit",lqlineedit_set_map)
    .def(constructor<>())
    .def(constructor<const QString&>())
    .def(constructor<const QString&,QWidget*>())
    .def("__call", lqlineedit_init)
    .def("__init", table_init_general<QLineEdit>)

    .property("text", &QLineEdit::text, &QLineEdit::setText)
    .property("inputMask", &QLineEdit::inputMask, &QLineEdit::setInputMask)
    .property("hasSelectedText", &QLineEdit::hasSelectedText)
    .property("selectedText", &QLineEdit::selectedText)
    .property("modified", &QLineEdit::isModified, &QLineEdit::setModified)
    .property("readOnly", &QLineEdit::isReadOnly, &QLineEdit::setReadOnly)
#if (QT_VERSION >= 0x407000) || defined(Q_WS_MAEMO_5)
    .property("placeholderText", &QLineEdit::placeholderText, &QLineEdit::setPlaceholderText)
#endif
    ;
}
