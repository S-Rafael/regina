
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

#import "NewPacketMenu.h"
#import "PacketTreeController.h"
#import "packet/packettype.h"

#pragma mark - NewPacketRow

// TODO: ios8: strange vertical line on the menu.

// The internal global array of all available packet types.
// This will be created the first time it is required.
static NSArray* _allRows;

@interface NewPacketRow : NSObject 

@property (strong, nonatomic, readonly) NSString* label;
@property (strong, nonatomic, readonly) NSString* icon;
@property (assign, nonatomic, readonly) regina::PacketType type;

- (id)initWithType:(regina::PacketType)type label:(NSString*)l icon:(NSString*)i;
+ (id)rowWithType:(regina::PacketType)type label:(NSString*)l icon:(NSString*)i;
+ (NSArray*)all;

@end

@implementation NewPacketRow

- (id)initWithType:(regina::PacketType)type label:(NSString *)l icon:(NSString *)i {
    self = [super init];
    if (self) {
        _type = type;
        _label = l;
        _icon = i;
    }
    return self;
}

+ (id)rowWithType:(regina::PacketType)type label:(NSString *)l icon:(NSString *)i {
    return [[NewPacketRow alloc] initWithType:type label:l icon:i];
}

+ (NSArray *)all {
    if (! _allRows)
        _allRows = [NSArray arrayWithObjects:
                    [NewPacketRow rowWithType:regina::PACKET_CONTAINER
                                        label:@"Container"
                                         icon:@"Container"],
                    [NewPacketRow rowWithType:regina::PACKET_DIM2TRIANGULATION
                                        label:@"2-D triangulation"
                                         icon:@"Dim2Triangulation"],
                    [NewPacketRow rowWithType:regina::PACKET_TRIANGULATION
                                        label:@"3-D triangulation"
                                         icon:@"Triangulation"],
                    [NewPacketRow rowWithType:regina::PACKET_NORMALSURFACELIST
                                        label:@"Normal surfaces"
                                         icon:@"Surfaces"],
                    [NewPacketRow rowWithType:regina::PACKET_ANGLESTRUCTURELIST
                                        label:@"Angle structures"
                                         icon:@"Angles"],
                    [NewPacketRow rowWithType:regina::PACKET_SNAPPEATRIANGULATION
                                        label:@"SnapPea triangulation"
                                         icon:@"SnapPea"],
                    [NewPacketRow rowWithType:regina::PACKET_SURFACEFILTER
                                        label:@"Surface filter"
                                         icon:@"Filter"],
                    [NewPacketRow rowWithType:regina::PACKET_TEXT
                                        label:@"Text"
                                         icon:@"Text"],
                    nil];
    return _allRows;
}

@end


#pragma mark - NewPacketMenu

@interface NewPacketMenu ()

@end

@implementation NewPacketMenu

- (void)viewDidLoad
{
    [super viewDidLoad];

    // Resize the popover to fit the available cells exactly.
    // Note that we cannot rely on [self.tableView rowHeight] to compute row heights,
    // since in iOS 8 the default rowHeight is -1 (UITableViewAutomaticDimension).
    NSInteger nRows = [self.tableView numberOfRowsInSection:0];

    CGFloat maxWidth = 0, height = 0;
    UITableViewCell* currCell;
    NSString *currLabel;
    CGFloat currWidth;
    for (NSInteger i = 0; i < nRows; ++i) {
        currCell = [self.tableView cellForRowAtIndexPath:[NSIndexPath indexPathForRow:i inSection:0]];
        height += CGRectGetHeight(currCell.frame);
        currLabel = currCell.textLabel.text;
        currWidth = [currLabel sizeWithAttributes:@{NSFontAttributeName:currCell.textLabel.font}].width;
        if (maxWidth < currWidth)
            maxWidth = currWidth;
    }
    CGFloat width = maxWidth + 80; // Room for 32x32 icon plus some padding.

    self.preferredContentSize = CGSizeMake(width, height);
    self.tableView.scrollEnabled = NO;
}

#pragma mark Table view

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return [NewPacketRow all].count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    // By the time we reach this point, the global array will have already been created.
    NewPacketRow* row = _allRows[indexPath.row];

    UITableViewCell* cell = [tableView dequeueReusableCellWithIdentifier:@"Entry" forIndexPath:indexPath];
    cell.textLabel.text = row.label;
    cell.imageView.image = [UIImage imageNamed:row.icon];
    return cell;
}

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
    return NO;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    [self.seguePopoverController dismissPopoverAnimated:NO];

    // By the time we reach this point, the global array will have already been created.
    NewPacketRow* row = _allRows[indexPath.row];
    [_packetTree newPacket:row.type];
}

@end
