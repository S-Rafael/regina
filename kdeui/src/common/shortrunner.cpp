
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  KDE User Interface                                                    *
 *                                                                        *
 *  Copyright (c) 1999-2011, Ben Burton                                   *
 *  For further details contact Ben Burton (bab@debian.org).              *
 *                                                                        *
 *  This program is free software; you can redistribute it and/or         *
 *  modify it under the terms of the GNU General Public License as        *
 *  published by the Free Software Foundation; either version 2 of the    *
 *  License, or (at your option) any later version.                       *
 *                                                                        *
 *  This program is distributed in the hope that it will be useful, but   *
 *  WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *  General Public License for more details.                              *
 *                                                                        *
 *  You should have received a copy of the GNU General Public             *
 *  License along with this program; if not, write to the Free            *
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,       *
 *  MA 02110-1301, USA.                                                   *
 *                                                                        *
 **************************************************************************/

/* end stub */

#include "shortrunner.h"

QString ShortRunner::run(bool mergeStderr) {
    // Start the child process running.
    proc.setOutputChannelMode(mergeStderr ?
        KProcess::MergedChannels : KProcess::OnlyStdoutChannel);
    connect(&proc, SIGNAL(started()), this, SLOT(processStarted()));
    connect(&proc, SIGNAL(finished(int, QProcess::ExitStatus)),
        this, SLOT(processFinished()));

    // Since we need to collect output, we must use start() and not
    // startDetached().
    proc.start();

    // Wait for it to finish, within a reasonable time limit.
    // Don't rely on the return value, since waitForFinished() can
    // also return false if the program already finished before this call.
    proc.waitForFinished(timeout * 1000);
    {
        QMutexLocker locker(&mutex);
        if (finished) {
            // All good.
            return proc.readAll();
        } else if (started)
            reachedTimeout = true;
    }

    // Bzzt.
    // Either we timed out or we never even started.  The member
    // reachedTimeout that we set above allows us to tell the difference.

    // Attempt to terminate (SIGQUIT) ...
    proc.terminate();
    // ... and if the program doesn't respond then kill it (SIGKILL).
    proc.waitForFinished(500);
    proc.kill();

    return QString::null;
}

void ShortRunner::processStarted() {
    QMutexLocker locker(&mutex);
    started = true;
}

void ShortRunner::processFinished() {
    QMutexLocker locker(&mutex);
    finished = true;
}

