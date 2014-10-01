
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  iOS User Interface                                                    *
 *                                                                        *
 *  Copyright (c) 1999-2013, Ben Burton                                   *
 *  For further details contact Ben Burton (bab@debian.org).              *
 *                                                                        *
 *  This program is free software; you can redistribute it and/or         *
 *  modify it under the terms of the GNU General Public License as        *
 *  published by the Free Software Foundation; either version 2 of the    *
 *  License, or (at your option) any later version.                       *
 *                                                                        *
 *  As an exception, when this program is distributed through (i) the     *
 *  App Store by Apple Inc.; (ii) the Mac App Store by Apple Inc.; or     *
 *  (iii) Google Play by Google Inc., then that store may impose any      *
 *  digital rights management, device limits and/or redistribution        *
 *  restrictions that are required by its terms of service.               *
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

#import "PacketTabBarController.h"
#import "PacketViewer.h"
#import "ReginaHelper.h"

// TODO: Too much vertical space at the top of each tab for iOS 7 (okay in iOS 8).

@interface PacketTabBarController () <UITabBarControllerDelegate>
@property (strong, nonatomic) NSString *defaultKey;
@end

@implementation PacketTabBarController

- (void)viewDidLoad
{
    self.delegate = self;

    if (! [ReginaHelper ios8]) {
        // Stop view from running beneath the tab bar on iOS 7.
        self.tabBar.translucent = NO;

        // Also, shade the tab bar (otherwise on iOS 7 it appears white).
        self.tabBar.barTintColor = [UIColor colorWithRed:0.975 green:0.975 blue:0.975 alpha:1.0];
    }
}

- (void)setSelectedImages:(NSArray *)imageNames
{
    for (int i = 0; i < imageNames.count; ++i)
        if (imageNames[i] != [NSNull null])
            static_cast<UITabBarItem*>(self.tabBar.items[i]).selectedImage = [UIImage imageNamed:imageNames[i]];
}

- (void)registerDefaultKey:(NSString *)key
{
    self.defaultKey = key;
    self.selectedIndex = [[NSUserDefaults standardUserDefaults] integerForKey:self.defaultKey];
}

- (void)tabBarController:(UITabBarController *)tabBarController didSelectViewController:(UIViewController *)viewController
{
    if (! self.defaultKey)
        return;

    NSUInteger index = [self.viewControllers indexOfObject:viewController];
    if (index != NSNotFound)
        [[NSUserDefaults standardUserDefaults] setInteger:index forKey:self.defaultKey];
}

- (void)endEditing
{
    UIViewController* c = self.selectedViewController;
    if ([c.class conformsToProtocol:@protocol(PacketEditor)])
        [static_cast<id<PacketEditor> >(c) endEditing];
}

@end
