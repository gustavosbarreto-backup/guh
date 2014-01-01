#include "actiontype.h"

ActionType::ActionType(const QUuid &id):
    m_id(id)
{
}

QUuid ActionType::id() const
{
    return m_id;
}

QString ActionType::name() const
{
    return m_name;
}

void ActionType::setName(const QString &name)
{
    m_name = name;
}