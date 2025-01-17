/******************************************************************************
 * $Id: wind_history.h, v1.0 2010/08/30 tom-r Exp $
 *
 * Project:  OpenCPN
 * Purpose:  Dashboard Plugin
 * Author:   Thomas Rauch
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.             *
 ***************************************************************************
 */

#ifndef __WIND_HISTORY_H__
#define __WIND_HISTORY_H__

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

// Warn: div by 0 if count == 1
#define WIND_RECORD_COUNT 3000

#include "instrument.h"

#include <wx/filename.h>


class DashboardInstrument_WindDirHistory: public DashboardInstrument
{
public:
    DashboardInstrument_WindDirHistory( wxWindow *parent, wxWindowID id, wxString title);
    ~DashboardInstrument_WindDirHistory(void);
    void SetData(unsigned long long st, double data, wxString unit, long long timestamp=0LL);
    void OnWindHistUpdTimer(wxTimerEvent &event);
    virtual void timeoutEvent(void){};
    wxSize GetSize( int orient, wxSize hint );

private:
    int m_SpdRecCnt;
    int m_DirRecCnt;
    int m_SpdStartVal;
    int m_DirStartVal;
    wxFileConfig  *m_pconfig;
    bool m_bWindSpeedUnitResetLogged;
    bool LoadConfig(void);
    bool SaveConfig(void);

    wxDECLARE_EVENT_TABLE();

protected:
    double alpha;
    double m_ArrayWindDirHistory[WIND_RECORD_COUNT];
    double m_ArrayWindSpdHistory[WIND_RECORD_COUNT];
    double m_ExpSmoothArrayWindSpd[WIND_RECORD_COUNT];
    double m_ExpSmoothArrayWindDir[WIND_RECORD_COUNT];
    wxDateTime::Tm m_ArrayRecTime[WIND_RECORD_COUNT];
            
    double m_MaxWindDir;
    double m_MinWindDir;
    double m_WindDirRange;
    double m_MaxWindSpd;  //...in array
    double m_TotalMaxWindSpd; // since O is started
    double m_WindDir;
    double m_WindSpd;
    double m_TrueWindDir;
    double m_TrueWindSpd;
    double m_MaxWindSpdScale;
    double m_ratioW;
    double m_oldDirVal;
    bool m_IsRunning;
    int m_SampleCount;
    wxString m_WindSpeedUnit;
    wxTimer *m_WindHistUpdTimer;
    wxRect m_WindowRect;
    wxRect m_DrawAreaRect; //the coordinates of the real darwing area
    int m_TopLineHeight;
    int m_width;
    int m_height;
    int m_LeftLegend;
    int m_RightLegend;
    wxString    m_logfile;        //for data export
    wxFile     *m_ostreamlogfile; //for data export
    bool        m_isExporting;      //for data export
    int         m_exportInterval; //for data export
    wxButton   *m_LogButton;     //for data export
    wxMenu     *m_pExportmenu;//for data export
    wxMenuItem *btn1Sec;
    wxMenuItem *btn5Sec;
    wxMenuItem *btn10Sec;
    wxMenuItem *btn20Sec;
    wxMenuItem *btn60Sec;

    void Draw(wxGCDC* dc);
    void DrawBackground(wxGCDC* dc);
    void DrawForeground(wxGCDC* dc);
    void SetMinMaxWindScale();
    void DrawWindDirScale(wxGCDC* dc);
    void DrawWindSpeedScale(wxGCDC* dc);
    wxString GetWindDirStr(wxString WindDir);
    void OnLogDataButtonPressed(wxCommandEvent& event);
    void ExportData(void);

};


#endif // __WIND_HISTORY_H__

