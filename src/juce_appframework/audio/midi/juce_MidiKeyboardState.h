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

#ifndef __JUCE_MIDIKEYBOARDSTATE_JUCEHEADER__
#define __JUCE_MIDIKEYBOARDSTATE_JUCEHEADER__

#include "juce_MidiBuffer.h"
#include "../../../juce_core/threads/juce_ScopedLock.h"
#include "../../../juce_core/containers/juce_VoidArray.h"
class MidiKeyboardState;


//==============================================================================
/**
    Receives events from a MidiKeyboardState object.

    @see MidiKeyboardState
*/
class JUCE_API  MidiKeyboardStateListener
{
public:
    //==============================================================================
    MidiKeyboardStateListener() throw()         {}
    virtual ~MidiKeyboardStateListener()        {}

    //==============================================================================
    /** Called when one of the MidiKeyboardState's keys is pressed.

        This will be called synchronously when the state is either processing a
        buffer in its MidiKeyboardState::processNextMidiBuffer() method, or
        when a note is being played with its MidiKeyboardState::noteOn() method.

        Note that this callback could happen from an audio callback thread, so be
        careful not to block, and avoid any UI activity in the callback.
    */
    virtual void handleNoteOn (MidiKeyboardState* source,
                               int midiChannel, int midiNoteNumber, float velocity) = 0;

    /** Called when one of the MidiKeyboardState's keys is released.

        This will be called synchronously when the state is either processing a
        buffer in its MidiKeyboardState::processNextMidiBuffer() method, or
        when a note is being played with its MidiKeyboardState::noteOff() method.

        Note that this callback could happen from an audio callback thread, so be
        careful not to block, and avoid any UI activity in the callback.
    */
    virtual void handleNoteOff (MidiKeyboardState* source,
                                int midiChannel, int midiNoteNumber) = 0;
};


//==============================================================================
/**
    Represents a piano keyboard, keeping track of which keys are currently pressed.

    This object can parse a stream of midi events, using them to update its idea
    of which keys are pressed for each individiual midi channel.

    When keys go up or down, it can broadcast these events to listener objects.

    It also allows key up/down events to be triggered with its noteOn() and noteOff()
    methods, and midi messages for these events will be merged into the
    midi stream that gets processed by processNextMidiBuffer().
*/
class JUCE_API  MidiKeyboardState
{
public:
    //==============================================================================
    MidiKeyboardState();
    ~MidiKeyboardState();

    //==============================================================================
    /** Resets the state of the object.

        All internal data for all the channels is reset, but no events are sent as a
        result.

        If you want to release any keys that are currently down, and to send out note-up
        midi messages for this, use the allNotesOff() method instead.
    */
    void reset();

    /** Returns true if the given midi key is currently held down for the given midi channel.

        The channel number must be between 1 and 16. If you want to see if any notes are
        on for a range of channels, use the isNoteOnForChannels() method.
    */
    bool isNoteOn (const int midiChannel, const int midiNoteNumber) const throw();

    /** Returns true if the given midi key is currently held down on any of a set of midi channels.

        The channel mask has a bit set for each midi channel you want to test for - bit
        0 = midi channel 1, bit 1 = midi channel 2, etc.

        If a note is on for at least one of the specified channels, this returns true.
    */
    bool isNoteOnForChannels (const int midiChannelMask, const int midiNoteNumber) const throw();

    /** Turns a specified note on.

        This will cause a suitable midi note-on event to be injected into the midi buffer during the
        next call to processNextMidiBuffer().

        It will also trigger a synchronous callback to the listeners to tell them that the key has
        gone down.
    */
    void noteOn (const int midiChannel, const int midiNoteNumber, const float velocity);

    /** Turns a specified note off.

        This will cause a suitable midi note-off event to be injected into the midi buffer during the
        next call to processNextMidiBuffer().

        It will also trigger a synchronous callback to the listeners to tell them that the key has
        gone up.

        But if the note isn't acutally down for the given channel, this method will in fact do nothing.
    */
    void noteOff (const int midiChannel, const int midiNoteNumber);

    /** This will turn off any currently-down notes for the given midi channel.

        If you pass 0 for the midi channel, it will in fact turn off all notes on all channels.

        Calling this method will make calls to noteOff(), so can trigger synchronous callbacks
        and events being added to the midi stream.
    */
    void allNotesOff (const int midiChannel);

    //==============================================================================
    /** Looks at a key-up/down event and uses it to update the state of this object.

        To process a buffer full of midi messages, use the processNextMidiBuffer() method
        instead.
    */
    void processNextMidiEvent (const MidiMessage& message);

    /** Scans a midi stream for up/down events and adds its own events to it.

        This will look for any up/down events and use them to update the internal state,
        synchronously making suitable callbacks to the listeners.

        If injectIndirectEvents is true, then midi events to produce the recent noteOn()
        and noteOff() calls will be added into the buffer.

        Only the section of the buffer whose timestamps are between startSample and
        (startSample + numSamples) will be affected, and any events added will be placed
        between these times.

        If you're going to use this method, you'll need to keep calling it regularly for
        it to work satisfactorily.

        To process a single midi event at a time, use the processNextMidiEvent() method
        instead.
    */
    void processNextMidiBuffer (MidiBuffer& buffer,
                                const int startSample,
                                const int numSamples,
                                const bool injectIndirectEvents);

    //==============================================================================
    /** Registers a listener for callbacks when keys go up or down.

        @see removeListener
    */
    void addListener (MidiKeyboardStateListener* const listener) throw();

    /** Deregisters a listener.

        @see addListener
    */
    void removeListener (MidiKeyboardStateListener* const listener) throw();

    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    CriticalSection lock;
    uint16 noteStates [128];
    MidiBuffer eventsToAdd;
    VoidArray listeners;

    void noteOnInternal  (const int midiChannel, const int midiNoteNumber, const float velocity);
    void noteOffInternal  (const int midiChannel, const int midiNoteNumber);

    MidiKeyboardState (const MidiKeyboardState&);
    const MidiKeyboardState& operator= (const MidiKeyboardState&);
};



#endif   // __JUCE_MIDIKEYBOARDSTATE_JUCEHEADER__
