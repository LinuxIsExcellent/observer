#ifndef STRINGTOTABLEVIEW_H
#define STRINGTOTABLEVIEW_H

#include <QDialog>
#include <QDebug>

extern "C"
{
     #include "lua.h"
     #include "lauxlib.h"
     #include "lualib.h"
}

namespace Ui {
class StringToTableView;
}

class StringToTableView : public QDialog
{
    Q_OBJECT

public:
    explicit StringToTableView(QWidget *parent = nullptr);
    void SetParam(int nCol, int nRow, QString str);

    ~StringToTableView();

    std::string ParseLuaTableToString(lua_State *L);
private:
    Ui::StringToTableView *ui;
};

#endif // STRINGTOTABLEVIEW_H
