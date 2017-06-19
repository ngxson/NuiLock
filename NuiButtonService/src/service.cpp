/*
 * Copyright (c) 2013-2015 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "service.hpp"

#include <bb/Application>
#include <bb/platform/Notification>
#include <bb/platform/NotificationDefaultApplicationSettings>
#include <bb/system/InvokeManager>
#include <bb/system/InvokeTargetReply>
#include <bb/platform/HomeScreen>
#include <bb/multimedia/MediaKeyWatcher>

#include <QTimer>

using namespace bb::platform;
using namespace bb::system;
using namespace bb::multimedia;

Service::Service() :
        QObject(),
        m_notify(new Notification(this)),
        m_invokeManager(new InvokeManager(this))
{
    m_invokeManager->connect(m_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)),
            this, SLOT(handleInvoke(const bb::system::InvokeRequest&)));

    isScreenLocked = false;

    MediaKeyWatcher* keyWatcher = new MediaKeyWatcher(MediaKey::PlayPause);
    connect(keyWatcher, SIGNAL(shortPress(bb::multimedia::MediaKey::Type)), this, SLOT(onMediaBtnPress()));


    NotificationDefaultApplicationSettings settings;
    settings.setPreview(NotificationPriorityPolicy::Allow);
    settings.apply();

    m_notify->setTitle("NuiButton Service");
    m_notify->setBody("NuiButton service requires attention");

    bb::system::InvokeRequest request;
    //request.setTarget("com.example.NuiButton");
    //request.setAction("bb.action.START");
    request.setTarget("sys.camera.card");
    request.setAction("bb.action.START");
    m_notify->setInvokeRequest(request);

    onTimeout();
}

void Service::handleInvoke(const bb::system::InvokeRequest & request)
{
    if (request.action().compare("com.example.NuiButtonService.RESET") == 0) {
        triggerNotification();
    }
}

void Service::onLockStateChanged(bb::platform::DeviceLockState::Type newState)
{
    if (newState == bb::platform::DeviceLockState::ScreenLocked ||
            newState == bb::platform::DeviceLockState::PasswordLocked ||
            newState == bb::platform::DeviceLockState::PinBlocked) {
        isScreenLocked = true;
    } else {
        isScreenLocked = false;
    }
}

void Service::triggerNotification()
{
    // Timeout is to give time for UI to minimize
    QTimer::singleShot(2000, this, SLOT(onTimeout()));
}

void Service::onMediaBtnPress() {
   qDebug() << "nui btn press";
   /*bb::system::InvokeManager *invokeManager = new InvokeManager(this);
   bb::system::InvokeRequest request;
   request.setTarget("com.example.NuiButton");
   request.setAction("bb.action.START");
   InvokeTargetReply *reply = invokeManager->invoke(request);*/

}

void Service::onTimeout()
{
    Notification::clearEffectsForAll();
    Notification::deleteAllFromInbox();
    m_notify->notify();
}
