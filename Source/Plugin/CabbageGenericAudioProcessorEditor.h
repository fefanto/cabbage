/*
  Copyright (C) 2012 Rory Walsh

  Cabbage is free software; you can redistribute it
  and/or modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  Cabbage is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with Csound; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307 USA

  This file is based on the JUCE library feil of the same name.
  JUCE - "Jules' Utility Class Extensions"
  Copyright 2004-9 by Raw Material Software Ltd.
*/


#ifndef __CABBAGE_GENERICAUDIOPROCESSOREDITOR_JUCEHEADER__
#define __CABBAGE_GENERICAUDIOPROCESSOREDITOR_JUCEHEADER__

//#include "FilterGraph.h"
#include "../CabbageLookAndFeel.h"


class ProcessorParameterPropertyComp   : public PropertyComponent,
                                         private AudioProcessorListener,
                                         private Timer
{
public:
    ProcessorParameterPropertyComp (const String& name, AudioProcessor& p, int paramIndex)
        : PropertyComponent (name),
          owner (p),
          index (paramIndex),
          paramHasChanged (false),
          slider (p, paramIndex),
		  lookAndFeelBasic(new CabbageLookAndFeelBasic())
    {
		setLookAndFeel(lookAndFeelBasic);
        startTimer (100);
        addAndMakeVisible (slider);
		setPreferredHeight(20);
        owner.addListener (this);
		slider.setLookAndFeel(lookAndFeelBasic);
		slider.lookAndFeelChanged();
    }

    ~ProcessorParameterPropertyComp()
    {
        owner.removeListener (this);
    }

    void refresh() override
    {
        paramHasChanged = false;

        if (slider.getThumbBeingDragged() < 0)
            slider.setValue (owner.getParameter (index), dontSendNotification);

        slider.updateText();
    }

    void paint(Graphics &g)
    {
        g.fillAll(cUtils::getComponentSkin().darker(.4f)); //background
        String text = getName();
        g.setColour(Colours::whitesmoke);

        Font font (cUtils::getTitleFont());
        //font.setFallbackFontName (String("Verdana")); //in case the user doesn't have the first font installed
        g.setFont (font);

        g.drawFittedText(text, 5, 5,
                         slider.getPosition().getX(), font.getHeight()-2, Justification::centred, 1);

        //	g.drawFittedText(text, 5, 5,
        //	font.getStringWidth(text), font.getHeight()-2, Justification::centredTop, 1);
    }
	
    void audioProcessorChanged (AudioProcessor*) override  {}

    void audioProcessorParameterChanged (AudioProcessor*, int parameterIndex, float) override
    {
        if (parameterIndex == index)
            paramHasChanged = true;
    }

    void timerCallback() override
    {
        if (paramHasChanged)
        {
            refresh();
            startTimerHz (50);
        }
        else
        {
            startTimer (jmin (1000 / 4, getTimerInterval() + 10));
        }
    }

private:
    //==============================================================================
    class ParamSlider  : public Slider
    {
    public:
        ParamSlider (AudioProcessor& p, int paramIndex)  : owner (p), index (paramIndex)
        {
            const int steps = owner.getParameterNumSteps (index);

            if (steps > 1 && steps < 0x7fffffff)
                setRange (0.0, 1.0, 1.0 / (steps - 1.0));
            else
                setRange (0.0, 1.0);

            setSliderStyle (Slider::LinearHorizontal);
            setTextBoxIsEditable (false);
            setScrollWheelEnabled (true);
        }

        void valueChanged() override
        {
            const float newVal = (float) getValue();

            if (owner.getParameter (index) != newVal)
            {
                owner.setParameterNotifyingHost (index, newVal);
                updateText();
            }
        }

        String getTextFromValue (double /*value*/) override
        {
            return owner.getParameterText (index) + " " + owner.getParameterLabel (index).trimEnd();
        }

    private:
        //==============================================================================
        AudioProcessor& owner;
        const int index;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParamSlider)
    };

    AudioProcessor& owner;
    const int index;
    bool volatile paramHasChanged;
    ParamSlider slider;
	ScopedPointer<CabbageLookAndFeelBasic> lookAndFeelBasic;
	
	

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProcessorParameterPropertyComp)
};


//==============================================================================

class CabbageGenericAudioProcessorEditor      : public AudioProcessorEditor
{
public:
	CabbageGenericAudioProcessorEditor(AudioProcessor* const p)
    : AudioProcessorEditor (p)
	{
		jassert (p != nullptr);
		setOpaque (true);

		addAndMakeVisible (panel);
		Array <PropertyComponent*> params;

		const int numParams = p->getNumParameters();
		int totalHeight = 0;

		for (int i = 0; i < numParams; ++i)
		{
			String name (p->getParameterName (i));
			if (name.trim().isEmpty())
				name = "Unnamed";

			ProcessorParameterPropertyComp* const pc = new ProcessorParameterPropertyComp (name, *p, i);
			params.add (pc);
			totalHeight += pc->getPreferredHeight();
		}

		panel.addProperties (params);

		setSize (400, jlimit (25, 400, totalHeight));
	}

	~CabbageGenericAudioProcessorEditor()
	{
	}

	void paint (Graphics& g)
	{
		g.fillAll (cUtils::getDarkerBackgroundSkin());
	}

	void resized()
	{
		panel.setBounds (getLocalBounds());
	}

private:
	PropertyPanel panel;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageGenericAudioProcessorEditor)
};



#endif

