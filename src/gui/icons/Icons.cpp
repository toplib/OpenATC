#include "Icons.h"

#include <QFile>
#include <QPainter>
#include <QSvgRenderer>

namespace Gui::Icons {

static QString resourcePath(Name name) {
  switch (name) {
  case Name::Plus:
    return QStringLiteral(":/icons/plus.svg");
  case Name::FolderOpen:
    return QStringLiteral(":/icons/folder-open.svg");
  case Name::Pencil:
    return QStringLiteral(":/icons/pencil.svg");
  case Name::Send:
    return QStringLiteral(":/icons/send.svg");
  case Name::Clock:
    return QStringLiteral(":/icons/clock.svg");
  case Name::Search:
    return QStringLiteral(":/icons/search.svg");
  case Name::Close:
    return QStringLiteral(":/icons/x.svg");
  case Name::User:
    return QStringLiteral(":/icons/user.svg");
  case Name::Link:
    return QStringLiteral(":/icons/link-2.svg");
  case Name::Plane:
    return QStringLiteral(":/icons/plane.svg");
  case Name::Pointer:
    return QStringLiteral(":/icons/mouse-pointer-2.svg");
  }
  return {};
}

QPixmap pixmap(Name name, const QColor &color, int size) {
  QFile file(resourcePath(name));
  if (!file.open(QIODevice::ReadOnly))
    return {};

  QByteArray svg = file.readAll();
  const QByteArray colorHex = color.name(QColor::HexRgb).toUtf8();
  svg.replace("currentColor", colorHex);
  svg.replace("stroke=\"#000\"", "stroke=\"" + colorHex + "\"");
  svg.replace("stroke=\"black\"", "stroke=\"" + colorHex + "\"");

  QSvgRenderer renderer(svg);
  if (!renderer.isValid())
    return {};

  QPixmap pm(size, size);
  pm.fill(Qt::transparent);
  QPainter painter(&pm);
  painter.setRenderHint(QPainter::Antialiasing, true);
  renderer.render(&painter);
  return pm;
}

QIcon icon(Name name, const QColor &color, int size) {
  return QIcon(pixmap(name, color, size));
}

} // namespace Gui::Icons
