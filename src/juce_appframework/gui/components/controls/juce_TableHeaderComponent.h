/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-7 by Raw Material Software ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the
   GNU General Public License, as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later version.

   JUCE is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with JUCE; if not, visit www.gnu.org/licenses or write to the
   Free Software Foundation, Inc., 59 Temple Place, Suite 330,
   Boston, MA 02111-1307 USA

  ------------------------------------------------------------------------------

   If you'd like to release a closed-source product which uses JUCE, commercial
   licenses are also available: visit www.rawmaterialsoftware.com/juce for
   more information.

  ==============================================================================
*/

#ifndef __JUCE_TABLEHEADERCOMPONENT_JUCEHEADER__
#define __JUCE_TABLEHEADERCOMPONENT_JUCEHEADER__

#include "../menus/juce_PopupMenu.h"
#include "../../../events/juce_AsyncUpdater.h"

class TableHeaderComponent;


//==============================================================================
/**
    Receives events from a TableHeaderComponent when columns are resized, moved, etc.

    You can register one of these objects for table events using TableHeaderComponent::addListener()
    and TableHeaderComponent::removeListener().

    @see TableHeaderComponent
*/
class JUCE_API  TableHeaderListener
{
public:
    //==============================================================================
    TableHeaderListener() {}

    /** Destructor. */
    virtual ~TableHeaderListener() {}

    //==============================================================================
    /** This is called when some of the table's columns are added, removed, hidden,
        or rearranged.
    */
    virtual void tableColumnsChanged (TableHeaderComponent* tableHeader) = 0;

    /** This is called when one or more of the table's columns are resized.
    */
    virtual void tableColumnsResized (TableHeaderComponent* tableHeader) = 0;

    /** This is called when the column by which the table should be sorted is changed.
    */
    virtual void tableSortOrderChanged (TableHeaderComponent* tableHeader) = 0;

    /** This is called when the user begins or ends dragging one of the columns around.

        When the user starts dragging a column, this is called with the ID of that
        column. When they finish dragging, it is called again with 0 as the ID.
    */
    virtual void tableColumnDraggingChanged (TableHeaderComponent* tableHeader,
                                             int columnIdNowBeingDragged);
};


//==============================================================================
/**
    A component that displays a strip of column headings for a table, and allows these
    to be resized, dragged around, etc.

    This is just the component that goes at the top of a table. You can use it
    directly for custom components, or to create a simple table, use the
    TableListBox class.

    To use one of these, create it and use addColumn() to add all the columns that you need.
    Each column must be given a unique ID number that's used to refer to it.

    @see TableListBox, TableHeaderListener
*/
class JUCE_API  TableHeaderComponent   : public Component,
                                         private AsyncUpdater
{
public:
    //==============================================================================
    /** Creates an empty table header.
    */
    TableHeaderComponent();

    /** Destructor. */
    ~TableHeaderComponent();

    //==============================================================================
    /** A combination of these flags are passed into the addColumn() method to specify
        the properties of a column.
    */
    enum ColumnPropertyFlags
    {
        visible                     = 1,    /**< If this is set, the column will be shown; if not, it will be hidden until the user enables it with the pop-up menu. */
        resizable                   = 2,    /**< If this is set, the column can be resized by dragging it. */
        draggable                   = 4,    /**< If this is set, the column can be dragged around to change its order in the table. */
        appearsOnColumnMenu         = 8,    /**< If this is set, the column will be shown on the pop-up menu allowing it to be hidden/shown. */
        sortable                    = 16,   /**< If this is set, then clicking on the column header will set it to be the sort column, and clicking again will reverse the order. */
        sortedForwards              = 32,   /**< If this is set, the column is currently the one by which the table is sorted (forwards). */
        sortedBackwards             = 64,   /**< If this is set, the column is currently the one by which the table is sorted (backwards). */

        /** This set of default flags is used as the default parameter value in addColumn(). */
        defaultFlags                = (visible | resizable | draggable | appearsOnColumnMenu | sortable),

        /** A quick way of combining flags for a column that's not resizable. */
        notResizable                = (visible | draggable | appearsOnColumnMenu | sortable),

        /** A quick way of combining flags for a column that's not resizable or sortable. */
        notResizableOrSortable      = (visible | draggable | appearsOnColumnMenu),

        /** A quick way of combining flags for a column that's not sortable. */
        notSortable                 = (visible | resizable | draggable | appearsOnColumnMenu)
    };

    /** Adds a column to the table.

        This will add a column, and asynchronously call the tableColumnsChanged() method of any
        registered listeners.

        @param columnName       the name of the new column. It's ok to have two or more columns with the same name
        @param columnId         an ID for this column. The ID can be any number apart from 0, but every column must have
                                a unique ID. This is used to identify the column later on, after the user may have
                                changed the order that they appear in
        @param width            the initial width of the column, in pixels
        @param maximumWidth     a maximum width that the column can take when the user is resizing it. This only applies
                                if the 'resizable' flag is specified for this column
        @param minimumWidth     a minimum width that the column can take when the user is resizing it. This only applies
                                if the 'resizable' flag is specified for this column
        @param propertyFlags    a combination of some of the values from the ColumnPropertyFlags enum, to define the
                                properties of this column
        @param insertIndex      the index at which the column should be added. A value of 0 puts it at the start (left-hand side)
                                and -1 puts it at the end (right-hand size) of the table. Note that the index the index within
                                all columns, not just the index amongst those that are currently visible
    */
    void addColumn (const String& columnName,
                    const int columnId,
                    const int width,
                    const int minimumWidth = 30,
                    const int maximumWidth = -1,
                    const int propertyFlags = defaultFlags,
                    const int insertIndex = -1);

    /** Removes a column with the given ID.

        If there is such a column, this will asynchronously call the tableColumnsChanged() method of any
        registered listeners.
    */
    void removeColumn (const int columnIdToRemove);

    /** Deletes all columns from the table.

        If there are any columns to remove, this will asynchronously call the tableColumnsChanged() method of any
        registered listeners.
    */
    void removeAllColumns();

    /** Returns the number of columns in the table.

        If onlyCountVisibleColumns is true, this will return the number of visible columns; otherwise it'll
        return the total number of columns, including hidden ones.

        @see isColumnVisible
    */
    int getNumColumns (const bool onlyCountVisibleColumns) const throw();

    /** Returns the name for a column.
        @see setColumnName
    */
    const String getColumnName (const int columnId) const throw();

    /** Changes the name of a column. */
    void setColumnName (const int columnId, const String& newName);

    /** Moves a column to a different index in the table.

        @param columnId             the column to move
        @param newVisibleIndex      the target index for it, from 0 to the number of columns currently visible.
    */
    void moveColumn (const int columnId, int newVisibleIndex);

    /** Changes the width of a column.

        This will cause an asynchronous callback to the tableColumnsResized() method of any registered listeners.
    */
    void setColumnWidth (const int columnId, const int newWidth);

    /** Shows or hides a column.

        This can cause an asynchronous callback to the tableColumnsChanged() method of any registered listeners.
        @see isColumnVisible
    */
    void setColumnVisible (const int columnId, const bool shouldBeVisible);

    /** Returns true if this column is currently visible.
        @see setColumnVisible
    */
    bool isColumnVisible (const int columnId) const;

    /** Changes the column which is the sort column.

        This can cause an asynchronous callback to the tableSortOrderChanged() method of any registered listeners.
        @see getSortColumnId, isSortedForwards
    */
    void setSortColumnId (const int columnId, const bool sortForwards);

    /** Returns the column ID by which the table is currently sorted, or 0 if it is unsorted.

        @see setSortColumnId, isSortedForwards
    */
    int getSortColumnId() const throw();

    /** Returns true if the table is currently sorted forwards, or false if it's backwards.
        @see setSortColumnId
    */
    bool isSortedForwards() const throw();

    //==============================================================================
    /** Returns the total width of all the visible columns in the table.
    */
    int getTotalWidth() const throw();

    /** Returns the index of a given column.

        If there's no such column ID, this will return -1.

        If onlyCountVisibleColumns is true, this will return the index amoungst the visible columns;
        otherwise it'll return the index amongst all the columns, including any hidden ones.
    */
    int getIndexOfColumnId (const int id, const bool onlyCountVisibleColumns) const throw();

    /** Returns the ID of the column at a given index.

        If onlyCountVisibleColumns is true, this will count the index amoungst the visible columns;
        otherwise it'll count it amongst all the columns, including any hidden ones.

        If the index is out-of-range, it'll return 0.
    */
    int getColumnIdOfIndex (int index, const bool onlyCountVisibleColumns) const throw();

    /** Returns the rectangle containing of one of the columns.

        The index is an index from 0 to the number of columns that are currently visible (hidden
        ones are not counted). It returns a rectangle showing the position of the column relative
        to this component's top-left. If the index is out-of-range, an empty rectangle is retrurned.
    */
    const Rectangle getColumnPosition (const int index) const throw();

    /** Finds the column ID at a given x-position in the component.

        If there is a column at this point this returns its ID, or if not, it will return 0.
    */
    int getColumnIdAtX (const int xToFind) const throw();

    /** If set to true, this indicates that the columns should be expanded or shrunk to fill the
        entire width of the component.

        By default this is disabled. Turning it on also means that when resizing a column, those
        on the right will be squashed to fit.
    */
    void setStretchToFitActive (const bool shouldStretchToFit);

    /** Returns true if stretch-to-fit has been enabled.
        @see setStretchToFitActive
    */
    bool isStretchToFitActive() const throw();

    /** If stretch-to-fit is enabled, this will resize all the columns to make them fit into the
        specified width, keeping their relative proportions the same.

        If the minimum widths of the columns are too wide to fit into this space, it may
        actually end up wider.
    */
    void resizeAllColumnsToFit (int targetTotalWidth);

    //==============================================================================
    /** Enables or disables the pop-up menu.

        The default menu allows the user to show or hide columns. You can add custom
        items to this menu by overloading the addMenuItems() and reactToMenuItem() methods.

        By default the menu is enabled.

        @see isPopupMenuActive, addMenuItems, reactToMenuItem
    */
    void setPopupMenuActive (const bool hasMenu);

    /** Returns true if the pop-up menu is enabled.
        @see setPopupMenuActive
    */
    bool isPopupMenuActive() const throw();

    //==============================================================================
    /** Returns a string that encapsulates the table's current layout.

        This can be restored later using restoreFromString(). It saves the order of
        the columns, the currently-sorted column, and the widths.

        @see restoreFromString
    */
    const String toString() const;

    /** Restores the state of the table, based on a string previously created with
        toString().

        @see toString
    */
    void restoreFromString (const String& storedVersion);

    //==============================================================================
    /** Adds a listener to be informed about things that happen to the header. */
    void addListener (TableHeaderListener* const newListener) throw();

    /** Removes a previously-registered listener. */
    void removeListener (TableHeaderListener* const listenerToRemove) throw();

    //==============================================================================
    /** This can be overridden to handle a mouse-click on one of the column headers.

        The default implementation will use this click to call getSortColumnId() and
        change the sort order.
    */
    virtual void columnClicked (int columnId, const ModifierKeys& mods);

    /** This can be overridden to add custom items to the pop-up menu.

        If you override this, you should call the superclass's method to add its
        column show/hide items, if you want them on the menu as well.

        Then to handle the result, override reactToMenuItem().

        @see reactToMenuItem
    */
    virtual void addMenuItems (PopupMenu& menu, const int columnIdClicked);

    /** Override this to handle any custom items that you have added to the
        pop-up menu with an addMenuItems() override.

        If the menuReturnId isn't one of your own custom menu items, you'll need to
        call TableHeaderComponent::reactToMenuItem() to allow the base class to
        handle the items that it had added.

        @see addMenuItems
    */
    virtual void reactToMenuItem (const int menuReturnId, const int columnIdClicked);

    //==============================================================================
    /** @internal */
    void paint (Graphics& g);
    /** @internal */
    void resized();
    /** @internal */
    void mouseMove (const MouseEvent&);
    /** @internal */
    void mouseEnter (const MouseEvent&);
    /** @internal */
    void mouseExit (const MouseEvent&);
    /** @internal */
    void mouseDown (const MouseEvent&);
    /** @internal */
    void mouseDrag (const MouseEvent&);
    /** @internal */
    void mouseUp (const MouseEvent&);
    /** @internal */
    const MouseCursor getMouseCursor();

    juce_UseDebuggingNewOperator

private:
    struct ColumnInfo
    {
        String name;
        int id, propertyFlags, width, minimumWidth, maximumWidth;
        double lastDeliberateWidth;

        bool isVisible() const throw();
    };

    OwnedArray <ColumnInfo> columns;
    Array <TableHeaderListener*> listeners;
    Component* dragOverlayComp;

    bool columnsChanged, columnsResized, sortChanged, menuActive, stretchToFit;
    int columnIdBeingResized, columnIdBeingDragged, initialColumnWidth;
    int columnIdUnderMouse, draggingColumnOffset, draggingColumnOriginalIndex, lastDeliberateWidth;

    ColumnInfo* getInfoForId (const int id) const throw();
    int visibleIndexToTotalIndex (const int visibleIndex) const throw();
    void sendColumnsChanged();
    void handleAsyncUpdate();
    void beginDrag (const MouseEvent&);
    void endDrag (const int finalIndex);
    int getResizeDraggerAt (const int mouseX) const throw();
    void updateColumnUnderMouse (int x, int y);
    void showColumnChooserMenu (const int);
    void resizeColumnsToFit (int firstColumnIndex, int targetTotalWidth);

    TableHeaderComponent (const TableHeaderComponent&);
    const TableHeaderComponent operator= (const TableHeaderComponent&);
};


#endif   // __JUCE_TABLEHEADERCOMPONENT_JUCEHEADER__
