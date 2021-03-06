/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginWrapperProcessor.h"
#include "PluginWrapperEditor.h"
#include "../CabbageUtils.h"


//==============================================================================
PluginWrapperEditor::PluginWrapperEditor (PluginWrapper& p)
    : AudioProcessorEditor (&p), processor (p)
{
    vstEditor = 0;
    if (getFilter()->vstInstance)
        vstEditor = getFilter()->vstInstance->createEditor();

    if (vstEditor)
    {
        addAndMakeVisible(vstEditor);
    }

    startTimer(100);

}

void PluginWrapperEditor::timerCallback()
{
    if (vstEditor)
    {
        setSize(vstEditor->getWidth(), vstEditor->getHeight());
        stopTimer();
    }

}

PluginWrapperEditor::~PluginWrapperEditor()
{
}

