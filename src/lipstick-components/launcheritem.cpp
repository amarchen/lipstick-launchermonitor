
// This file is part of lipstick, a QML desktop library
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation
// and appearing in the file LICENSE.LGPL included in the packaging
// of this file.
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// Copyright (c) 2011, Robin Burchell
// Copyright (c) 2012, Timur Kristóf <venemo@fedoraproject.org>

#include <QDebug>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QTimer>
#include <mlite5/mlite-global.h>
#include <mlite5/MDesktopEntry>

#ifdef HAVE_CONTENTACTION
#include <contentaction.h>
#endif

#include "launcheritem.h"

LauncherItem::LauncherItem(const QString &filePath, QObject *parent)
    : QObject(parent)
    , _isLaunching(false)
    , _customIconFilename("")
    , _serial(0)
{
    if (!filePath.isEmpty()) {
        setFilePath(filePath);
    }

    // TODO: match the PID of the window thumbnails with the launcher processes
    // Launching animation will be disabled if the window of the launched app shows up
}

LauncherItem::~LauncherItem()
{
}

void LauncherItem::setFilePath(const QString &filePath)
{
    if (!filePath.isEmpty()) {
        _desktopEntry = QSharedPointer<MDesktopEntry>(new MDesktopEntry(filePath));
    } else {
        _desktopEntry.clear();
    }

    emit this->itemChanged();
}

QString LauncherItem::filePath() const
{
    return !_desktopEntry.isNull() ? _desktopEntry->fileName() : QString();
}

QString LauncherItem::exec() const
{
    return !_desktopEntry.isNull() ? _desktopEntry->exec() : QString();
}

QString LauncherItem::title() const
{
    return !_desktopEntry.isNull() ? _desktopEntry->name() : QString();
}

QString LauncherItem::entryType() const
{
    return !_desktopEntry.isNull() ? _desktopEntry->type() : QString();
}

QString LauncherItem::iconId() const
{
    if (!_customIconFilename.isEmpty()) {
        return QString("%1#serial=%2").arg(_customIconFilename).arg(_serial);
    }

    return getOriginalIconId();
}

QStringList LauncherItem::desktopCategories() const
{
    return !_desktopEntry.isNull() ? _desktopEntry->categories() : QStringList();
}

QString LauncherItem::titleUnlocalized() const
{
    return !_desktopEntry.isNull() ? _desktopEntry->nameUnlocalized() : QString();
}

bool LauncherItem::shouldDisplay() const
{
    return !_desktopEntry.isNull() ? !_desktopEntry->noDisplay() : false;
}

bool LauncherItem::isValid() const
{
    return !_desktopEntry.isNull() ? _desktopEntry->isValid() : false;
}

bool LauncherItem::isLaunching() const
{
    return _isLaunching;
}

void LauncherItem::setIsLaunching(bool isLaunching)
{
    if (_isLaunching != isLaunching) {
        _isLaunching = isLaunching;
        emit this->isLaunchingChanged();
    }
}

void LauncherItem::launchApplication()
{
    if (_desktopEntry.isNull())
        return;

#if defined(HAVE_CONTENTACTION)
    LAUNCHER_DEBUG("launching content action for" << _desktopEntry->name());
    ContentAction::Action action = ContentAction::Action::launcherAction(_desktopEntry, QStringList());
    action.trigger();
#else
    LAUNCHER_DEBUG("launching exec line for" << _desktopEntry->name());

    // Get the command text from the desktop entry
    QString commandText = _desktopEntry->exec();

    // Take care of the freedesktop standards things

    commandText.replace(QRegExp("%k"), filePath());
    commandText.replace(QRegExp("%c"), _desktopEntry->name());
    commandText.remove(QRegExp("%[fFuU]"));

    if (!_desktopEntry->icon().isEmpty())
        commandText.replace(QRegExp("%i"), QString("--icon ") + _desktopEntry->icon());

    // DETAILS: http://standards.freedesktop.org/desktop-entry-spec/latest/index.html
    // DETAILS: http://standards.freedesktop.org/desktop-entry-spec/latest/ar01s06.html

    // Launch the application
    QProcess::startDetached(commandText);
#endif

    setIsLaunching(true);

    // This is a failsafe to allow launching again after 5 seconds in case the application crashes on startup and no window is ever created
    QTimer::singleShot(5000, this, SLOT(setIsLaunching()));
}

bool LauncherItem::isStillValid()
{
    // Force a reload of _desktopEntry
    setFilePath(filePath());
    return isValid();
}

QString LauncherItem::getOriginalIconId() const
{
    return !_desktopEntry.isNull() ? _desktopEntry->icon() : QString();
}

void LauncherItem::setIconFilename(const QString &path)
{
    _customIconFilename = path;
    _serial++;
    emit itemChanged();
}

QString LauncherItem::iconFilename() const
{
    return _customIconFilename;
}
