#pragma once

#include <QColor>
#include <QIcon>
#include <QPixmap>
#include <QString>

namespace Gui::Icons {

enum class Name {
  Plus,
  FolderOpen,
  Pencil,
  Send,
  Clock,
  Search,
  Close,
  User,
  Link,
  Plane,
  Pointer,
};

QIcon icon(Name name, const QColor &color = QColor(QStringLiteral("#8a8a8a")),
           int size = 16);

QPixmap pixmap(Name name, const QColor &color = QColor(QStringLiteral("#8a8a8a")),
               int size = 16);

} // namespace Gui::Icons
