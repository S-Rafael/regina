
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

#import "PacketManagerIOS.h"
#import "ScriptViewController.h"
#import "packet/nscript.h"

// TODO: Do we need to fix the "ambiguous row height" warning under iOS 8?
// TODO: Check behaviour of table row separators under iOS 8.
// TODO: The code windows scrolls down a little on load in landspace mode.

#pragma mark - Script variable cell

@interface ScriptVariableCell ()
@property (weak, nonatomic) IBOutlet UILabel *variable;
@property (weak, nonatomic) IBOutlet UILabel *value;
@property (weak, nonatomic) IBOutlet UIImageView *icon;
@end

@implementation ScriptVariableCell
@end

#pragma mark - Script view controller

@interface ScriptViewController () <UITableViewDataSource, UITableViewDelegate> {
    CGFloat variableHeaderHeight;
}
@property (weak, nonatomic) IBOutlet UITableView *variables;
@property (weak, nonatomic) IBOutlet UITextView *script;
@end

@implementation ScriptViewController

- (void)viewDidLoad
{
    [super viewDidLoad];

    regina::NScript* s = static_cast<regina::NScript*>(self.packet);
    self.script.text = [NSString stringWithUTF8String:s->getText().c_str()];

    variableHeaderHeight = 0;

    self.variables.delegate = self;
    self.variables.dataSource = self;
}

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath
{
    return NO;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    regina::NScript* s = static_cast<regina::NScript*>(self.packet);

    if (indexPath.row == 0)
        return [tableView dequeueReusableCellWithIdentifier:@"Header"];

    if (s->getNumberOfVariables() == 0)
        return [tableView dequeueReusableCellWithIdentifier:@"Empty"];

    ScriptVariableCell *cell = [tableView dequeueReusableCellWithIdentifier:@"Variable" forIndexPath:indexPath];
    cell.variable.text = [NSString stringWithUTF8String:s->getVariableName(indexPath.row - 1).c_str()];

    regina::NPacket* value = s->getVariableValue(indexPath.row - 1);
    if (value) {
        cell.icon.image = [PacketManagerIOS iconFor:value];
        if (value->getTreeParent())
            cell.value.text = [NSString stringWithUTF8String:value->getPacketLabel().c_str()];
        else
            cell.value.text = @"⟨Entire tree⟩";
    } else {
        cell.icon.image = nil;
        cell.value.text = @"⟨None⟩";
    }
    return cell;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    unsigned long nVar = static_cast<regina::NScript*>(self.packet)->getNumberOfVariables();
    return 1 + (nVar > 0 ? nVar : 1);
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (indexPath.row > 0)
        return self.variables.rowHeight;

    // The header row is smaller.  Calculate it based on the cell contents, which include
    // auto-layout constraints that pin the labels to the upper and lower boundaries.
    if (variableHeaderHeight == 0) {
        UITableViewCell* cell = [self.variables dequeueReusableCellWithIdentifier:@"Header"];
        [cell layoutIfNeeded];
        CGSize size = [cell.contentView systemLayoutSizeFittingSize:UILayoutFittingCompressedSize];
        variableHeaderHeight = size.height;
    }
    return variableHeaderHeight;
}

@end