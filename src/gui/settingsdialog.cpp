// 
//  Copyright 2012 Andrew Okin
// 
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
// 
//        http://www.apache.org/licenses/LICENSE-2.0
// 
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//

#include "settingsdialog.h"

SettingsDialog::SettingsDialog(wxWindow *parent, wxWindowID id)
	: wxDialog(parent, id, _T("Settings"), wxDefaultPosition,
		wxSize(500, 450))
{
	wxBoxSizer *mainBox = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainBox);
	
	// Tab control
	tabCtrl = new wxNotebook(this, -1, wxDefaultPosition, 
							 wxDefaultSize, wxNB_LEFT);
	mainBox->Add(tabCtrl, 1, wxEXPAND | wxALL, 8);
	
	// General tab
	wxPanel *generalPanel = new wxPanel(tabCtrl, -1);
	wxGridBagSizer *generalBox = new wxGridBagSizer();
	generalPanel->SetSizer(generalBox);
	tabCtrl->AddPage(generalPanel, _T("General"), true);
	
	// Console settings group box
	wxStaticBoxSizer *consoleBox = new wxStaticBoxSizer(wxVERTICAL, 
		generalPanel, _T("Console Settings"));
	showConsoleCheck = new wxCheckBox(generalPanel, -1, 
		_T("Show console while an instance is running."));
	consoleBox->Add(showConsoleCheck, 0, wxALL, 4);
	autoCloseConsoleCheck = new wxCheckBox(generalPanel, -1,
		_T("Automatically close console when the game quits."));
	consoleBox->Add(autoCloseConsoleCheck, 0, wxALL, 4);
	generalBox->Add(consoleBox, wxGBPosition(0, 0), wxGBSpan(1, 1), wxALL | wxEXPAND, 4);
	
	// Update settings group box
	wxStaticBoxSizer *updateBox = new wxStaticBoxSizer(wxVERTICAL,
		generalPanel, _T("Update Settings"));
	autoUpdateCheck = new wxCheckBox(generalPanel, -1,
		_T("Check for updates when MultiMC starts?"));
	updateBox->Add(autoUpdateCheck, 0, wxALL, 4);
	forceUpdateToggle = new wxToggleButton(generalPanel, -1,
		_T("Force-update MultiMC"));
	updateBox->Add(forceUpdateToggle, 0, wxALL, 4);
	generalBox->Add(updateBox, wxGBPosition(1, 0), wxGBSpan(1, 1), wxALL | wxEXPAND, 4);
	
	// Instance directory group box
	wxStaticBoxSizer *instDirBox = new wxStaticBoxSizer(wxHORIZONTAL,
		generalPanel, _T("Instance Folder"));
	instDirTextBox = new wxTextCtrl(generalPanel, -1);
	instDirBox->Add(instDirTextBox, 1, wxEXPAND | wxALL, 4);
	wxButton *instDirBrowseButton = new wxButton(generalPanel, 
		ID_BrowseInstDir, _T("Browse..."));
	instDirBox->Add(instDirBrowseButton, 0, wxALL, 4);
	generalBox->Add(instDirBox, wxGBPosition(2, 0), wxGBSpan(1, 1), wxALL | wxEXPAND, 4);
	
	generalBox->SetSizeHints(generalPanel);
	
	// Advanced tab
	wxPanel *advancedPanel = new wxPanel(tabCtrl, -1);
	wxGridBagSizer *advancedBox = new wxGridBagSizer();
	advancedPanel->SetSizer(advancedBox);
	tabCtrl->AddPage(advancedPanel, _T("Advanced"), false);
	
	// Memory group box
	wxStaticBoxSizer *memoryBox = new wxStaticBoxSizer(wxVERTICAL,
		advancedPanel, _T("Memory"));
	advancedBox->Add(memoryBox, wxGBPosition(0, 0), wxGBSpan(1, 1), wxALL | wxEXPAND, 4);
	
	// Min memory
	wxBoxSizer *minMemBox = new wxBoxSizer(wxHORIZONTAL);
	memoryBox->Add(minMemBox, 0, wxALL, 4);
	wxStaticText *minMemLabel = new wxStaticText(advancedPanel, -1, 
		_T("Minimum memory allocation: "));
	minMemBox->Add(minMemLabel, 1, wxEXPAND);
	minMemorySpin = new wxSpinCtrl(advancedPanel, -1);
	minMemorySpin->SetRange(256, Utils::GetMaxAllowedMemAlloc());
	minMemBox->Add(minMemorySpin);
	
	// Max memory
	wxBoxSizer *maxMemBox = new wxBoxSizer(wxHORIZONTAL);
	memoryBox->Add(maxMemBox, 0, wxALL, 4);
	wxStaticText *maxMemLabel = new wxStaticText(advancedPanel, -1, 
		_T("Maximum memory allocation: "));
	maxMemBox->Add(maxMemLabel, 1, wxEXPAND);
	maxMemorySpin = new wxSpinCtrl(advancedPanel, -1);
	maxMemorySpin->SetRange(512, Utils::GetMaxAllowedMemAlloc());
	maxMemBox->Add(maxMemorySpin);
	
	// Java path
	wxStaticBoxSizer *javaPathBox = new wxStaticBoxSizer(wxHORIZONTAL, 
		advancedPanel, _T("Java path"));
	javaPathTextBox = new wxTextCtrl(advancedPanel, -1);
	javaPathBox->Add(javaPathTextBox, 1, wxALL | wxEXPAND, 4);
	advancedBox->Add(javaPathBox, wxGBPosition(1, 0), wxGBSpan(1, 1), wxALL | wxEXPAND, 4);
	
	
	// Buttons
	wxSizer *btnBox = CreateButtonSizer(wxOK | wxCANCEL);
	mainBox->Add(btnBox, 0, wxALIGN_RIGHT | wxBOTTOM | wxRIGHT, 8);

	LoadSettings();
}

void SettingsDialog::OnButtonClicked(wxCommandEvent& event)
{
	this->Close();
}

void SettingsDialog::ApplySettings(AppSettings &s /* = settings */)
{
	s.showConsole = showConsoleCheck->IsChecked();
	s.autoCloseConsole = autoCloseConsoleCheck->IsChecked();
	
	s.autoUpdate = autoUpdateCheck->IsChecked();
	
	wxFileName newInstDir = wxFileName::DirName(instDirTextBox->GetValue());
	if (!s.instanceDir.SameAs(newInstDir))
	{
		wxFileName oldInstDir = s.instanceDir;
		
		int response = wxMessageBox(_T("You've changed your instance \
			directory, would you like to transfer all of your instances?"),
			_T("Instance directory changed."), 
			wxYES | wxNO | wxCANCEL | wxCENTER, this);
		
	RetryTransfer:
		if (response != wxID_CANCEL)
		{
			s.instanceDir = newInstDir;
		}
		
		if (response == wxID_YES)
		{
			// TODO Move all instances to new instance directory
		}
	}

	s.minMemAlloc = minMemorySpin->GetValue();
	s.maxMemAlloc = maxMemorySpin->GetValue();

	s.javaPath = javaPathTextBox->GetValue().c_str();
	
	s.Save();
}

void SettingsDialog::LoadSettings(AppSettings &s /* = settings */)
{
	showConsoleCheck->SetValue(s.showConsole);
	autoCloseConsoleCheck->SetValue(s.autoCloseConsole);

	autoUpdateCheck->SetValue(s.autoUpdate);

	instDirTextBox->SetValue(s.instanceDir.GetFullPath());

	minMemorySpin->SetValue(s.minMemAlloc);
	maxMemorySpin->SetValue(s.maxMemAlloc);

	javaPathTextBox->SetValue(s.javaPath.GetFullPath());
}

void SettingsDialog::OnBrowseInstDirClicked(wxCommandEvent& event)
{
	wxDirDialog *dirDlg = new wxDirDialog(this, _("Select a new instance folder."), 
		instDirTextBox->GetValue());
	if (dirDlg->ShowModal() == wxID_OK)
		instDirTextBox->ChangeValue(dirDlg->GetPath());
}


BEGIN_EVENT_TABLE(SettingsDialog, wxDialog)
	EVT_BUTTON(ID_BrowseInstDir, SettingsDialog::OnBrowseInstDirClicked)
END_EVENT_TABLE()