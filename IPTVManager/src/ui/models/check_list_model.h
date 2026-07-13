#pragma once

#include <QAbstractListModel>
#include <QJsonArray>
#include <QJsonObject>

namespace Iptv::Ui {

/**
 * @brief 检测列表模型
 *        将QJsonArray转换为QML可用的ListModel，用于频道源检查下拉框
 */
class CheckListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /** @brief 角色定义 */
    enum Roles {
        BracketRole = Qt::UserRole, ///< 序号括号，如"[  1]"
        NameRole,                  ///< 频道名称
        TypeRole,                  ///< 源类型（TS/SD/HD/4K/8K）
        IpPortRole,                ///< IP:端口
        FullTextRole               ///< 完整显示文本
    };

    explicit CheckListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    /** @brief 设置检测列表数据
     *  @param list JSON数组，每个元素包含 name, type, ip, port 字段 */
    void setList(const QJsonArray &list);

    /** @brief 获取原始数据 */
    QJsonArray list() const;

private:
    QJsonArray m_list;  ///< 检测列表数据
};

} // namespace Iptv::Ui
