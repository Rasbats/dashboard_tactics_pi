/***************************************************************************
* $Id: polarcompass.cpp, v1.0 2016/06/07 tom_BigSpeedy Exp $
*
* Project:  OpenCPN
* Purpose:  tactics Plugin
* Author:   Thomas Rauch
*     
***************************************************************************
*   Copyright (C) 2010 by David S. Register                               *
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
*   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
***************************************************************************
*/

#include "polarcompass.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <cmath>

#include "wx/tokenzr.h"

#include "Polar.h"

#include "TacticsFunctions.h"

#include "tactics_pi_ext.h"
#include "dashboard_pi_ext.h"

/***************************************************************************************
****************************************************************************************/
TacticsInstrument_PolarCompass::TacticsInstrument_PolarCompass(wxWindow *parent, wxWindowID id, wxString title, unsigned long long cap_flag) :
DashboardInstrument_Dial(parent, id, title, cap_flag, 0, 360, 0, 360)
{
	SetOptionMarker(5, DIAL_MARKER_SIMPLE, 2);
	SetOptionLabel(20, DIAL_LABEL_ROTATED);
	SetOptionMainValue(_T("%.0f"), DIAL_POSITION_INSIDE);
	//    Get a pointer to the opencpn configuration object

	m_pconfig = GetOCPNConfigObject();

	m_Bearing = std::nan("1");
	m_ExtraValueDTW = std::nan("1");
	m_CurrDir = std::nan("1");
	m_CurrSpeed = std::nan("1");
    m_currAngleStart = std::nan("1");
	m_TWA = std::nan("1");
	m_TWD = std::nan("1");
	m_AWA = -999;
	m_TWS = std::nan("1");
	m_Hdt = -999.9;
	m_Leeway = 0;
	m_PolSpd = std::nan("1");
	m_PolSpd_Percent = std::nan("1");
	m_diffCogHdt = 0.0;
    m_lat = std::nan("1");
    m_lon = std::nan("1");
	m_StW = 0.0;
	m_predictedSog = std::nan("1");
    m_BearingUnit = wxEmptyString;
    m_ExtraValueDTWUnit = wxEmptyString;
    m_ToWpt = _T("---");
    m_CurrDirUnit = wxEmptyString;
    m_CurrSpeedUnit = wxEmptyString;
    m_StWUnit = wxEmptyString;
    m_curTack = wxEmptyString;
    m_targetTack = wxEmptyString;
    m_LeewayUnit = wxEmptyString;
    m_ExpSmoothDegRange = 0.0; 
    alpha_diffCogHdt = 0.1;
    m_LaylineDegRange = 0.0;
    for (int i = 0; i < COGRANGE; i++)
        m_COGRange[i] = 0.0;
    m_Cog = -999.9;
    m_ExpSmoothDiffCogHdt = 0.0;
    m_oldExpSmoothDiffCogHdt = 0.0;
    mExpSmDegRange = new ExpSmooth(g_dalphaDeltCoG);

    m_timeout = false;
    
	LoadConfig();

}
/***************************************************************************************
****************************************************************************************/
void TacticsInstrument_PolarCompass::SetData(unsigned long long st,
                                             double data, wxString unit, long long timestamp )
{
    /* We receive data from boat's instrument but also Tactica function's virtual
       instrument data. They can get interrupted if some source data is missing.
       However, lat/lon do no get normally interrupted (chart position).
    */
    bool receivedfuncdata = false;

 	if (st == OCPN_DBP_STC_COG) {
		m_Cog = data;
        if ( !std::isnan( data ) )
            receivedfuncdata = true;
	}
	else if (st == OCPN_DBP_STC_HDT) {
        m_Hdt = data;
        if ( !std::isnan( data ) ) {
            m_AngleStart = -data;
            m_MainValue = data;
            m_MainValueUnit = unit;
            receivedfuncdata = true;
        }
        else {
            m_AngleStart = 0;
            m_MainValue = std::nan("1");
            m_MainValueUnit = _T("");
        }
	}
	else if (st == OCPN_DBP_STC_CURRDIR) {
		m_CurrDir = data;
        if ( !std::isnan( data ) )
            receivedfuncdata = true;
		m_CurrDirUnit = unit;
	}
	else if (st == OCPN_DBP_STC_CURRSPD) {
		m_CurrSpeed = data;
        if ( !std::isnan( data ) )
            receivedfuncdata = true;
		m_CurrSpeedUnit = unit;
	}

	else if (st == OCPN_DBP_STC_DTW) {
		if ( !GetSingleWaypoint( g_sMarkGUID, m_pMark ) ) {
            m_ExtraValueDTW = data;
            if ( !std::isnan( data ) ) {
                m_ExtraValueDTWUnit = unit;
                receivedfuncdata = true;
            }
            else
                m_ExtraValueDTWUnit = _T("");
		}
	}
	else if (st == OCPN_DBP_STC_TWA) {
		m_TWA = data;
        if ( !std::isnan( data ) )
            receivedfuncdata = true;
		m_curTack = unit;
	}
    else if (st == OCPN_DBP_STC_TWD) {
        m_TWD = data;
        if ( !std::isnan( data ) )
            receivedfuncdata = true;
    }
    else if (st == OCPN_DBP_STC_AWA) {
		m_AWA = data;
        if ( !std::isnan( data ) )
            receivedfuncdata = true;
	}
	else if (st == OCPN_DBP_STC_TWS) {
		m_TWS = data;
        if ( !std::isnan( data ) )
            receivedfuncdata = true;
	}

	else if (st == OCPN_DBP_STC_LEEWAY) {
		m_Leeway = data;
        if ( !std::isnan( data ) )
            receivedfuncdata = true;
        m_LeewayUnit = unit;
	}
	else if (st == OCPN_DBP_STC_LAT) {
		m_lat = data;
	}
	else if (st == OCPN_DBP_STC_LON) {
		m_lon = data;
	}
	else if (st == OCPN_DBP_STC_STW) {
		m_StW = data;
        if ( !std::isnan( data ) )
            receivedfuncdata = true;
	}

	if (m_Cog != -999 && m_Hdt != -999){
		m_diffCogHdt = m_Cog - m_Hdt;
    }
	if (st == OCPN_DBP_STC_BRG) {
        m_Bearing = data;
        if ( !std::isnan( data ) )
            receivedfuncdata = true;
        m_ToWpt = unit;
		m_BearingUnit = _T("\u00B0");
	}
    if (!GetSingleWaypoint(g_sMarkGUID, m_pMark))
        m_pMark = NULL;
    if (m_pMark && !std::isnan(m_lat) && !std::isnan(m_lon)) {
      double dist;
      DistanceBearingMercator_Plugin(m_pMark->m_lat, m_pMark->m_lon, m_lat, m_lon, &m_Bearing, &dist);
      m_ToWpt = g_sMarkGUID;
      m_ExtraValueDTW = toUsrDistance_Plugin(dist, g_iDashDistanceUnit);
      m_ExtraValueDTWUnit = getUsrDistanceUnit_Plugin(g_iDashDistanceUnit);
      m_BearingUnit = _T("\u00B0");
    }
    if (!m_pMark && std::isnan(m_Bearing)){
      m_ToWpt = _T("---");
      m_ExtraValueDTW = std::nan("1");
      m_predictedSog = std::nan("1");
      m_ExtraValueDTWUnit = getUsrDistanceUnit_Plugin(g_iDashDistanceUnit);
      m_BearingUnit = _T("\u00B0");
    }

    if ( receivedfuncdata ) {
        m_timeout = false;
        setTimestamp( timestamp );
    }

	CalculateLaylineDegreeRange();
}

void TacticsInstrument_PolarCompass::derivedTimeoutEvent()
{
    m_timeout = true;
    m_MainValue = std::nan("1");
    m_MainValueUnit = _T("");
    m_Bearing = std::nan("1");
    m_CurrDir = std::nan("1");
    m_CurrSpeed = std::nan("1");
    m_currAngleStart = std::nan("1");
    m_CurrDirUnit = wxEmptyString;
    m_ToWpt = _T("---");
    m_ExtraValueDTW = std::nan("1");
    m_predictedSog = std::nan("1");
    m_ExtraValueDTWUnit = getUsrDistanceUnit_Plugin(g_iDashDistanceUnit);
    m_BearingUnit = _T("\u00B0");
    m_ExtraValueDTW = std::nan("1");
    m_TWA = std::nan("1");
    m_AWA = -999;
    m_StW = std::nan("1");
}
/***************************************************************************************
****************************************************************************************/
void TacticsInstrument_PolarCompass::Draw(wxGCDC* bdc)
{
	wxColour c1;
	GetGlobalColor( g_sDialColorBackground, &c1 );
	wxBrush b1(c1);
	bdc->SetBackground(b1);
	bdc->Clear();

	wxSize size = GetClientSize();
	m_cx = size.x / 2;
	int availableHeight = size.y - m_TitleHeight - 6;
	int width, height;
	bdc->GetTextExtent(_T("000"), &width, &height, 0, 0, g_pFontLabel);
	m_cy = m_TitleHeight + 2;
	m_cy += availableHeight / 2;
	m_radius = availableHeight / 2. *0.80;


	DrawLabels(bdc);
	DrawFrame(bdc);
	DrawBackground(bdc);
    DrawForeground(bdc);
    if (!std::isnan(m_Bearing) && !std::isnan(m_ExtraValueDTW)){
        //DrawData(bdc, m_Bearing, m_BearingUnit, _T("BRG:%.f"), DIAL_POSITION_TOPLEFT);
        DrawData(bdc, m_ExtraValueDTW, m_ExtraValueDTWUnit, _T("DTW:%.1f"), DIAL_POSITION_TOPLEFT);
        DrawData(bdc, 0, m_ToWpt, _T(""), DIAL_POSITION_TOPRIGHT);
    }
    else {
        DrawData(bdc, 0, _T(""), _T(""), DIAL_POSITION_TOPLEFT);
        DrawData(bdc, 0, _T(""), _T(""), DIAL_POSITION_TOPRIGHT);
    }
    if (!std::isnan(m_TWA) && !std::isnan(m_TWS) ){
        if ( !BoatPolar->isValid() ) {
            m_PolSpd = std::nan("1");
        }
        else {
            m_PolSpd = BoatPolar->GetPolarSpeed(m_TWA, m_TWS);
        }
        if ( !std::isnan(m_PolSpd) && (m_PolSpd > 0.) ) {
            m_PolSpd_Percent = fromUsrSpeed_Plugin(
                m_StW, g_iDashSpeedUnit) / m_PolSpd * 100;
            if ( std::isnan(m_PolSpd_Percent) ||
                 (m_PolSpd_Percent >= POLAR_PERFORMANCE_PERCENTAGE_LIMIT) )
                m_PolSpd_Percent = POLAR_PERFORMANCE_PERCENTAGE_LIMIT;
        }
        else
            m_PolSpd = m_PolSpd_Percent = std::nan("1");
    }
    else{
        m_PolSpd = m_PolSpd_Percent = std::nan("1");
    }
    if ( !std::isnan( m_StW ) )
        DrawData(bdc, m_StW, m_StWUnit, _T("STW:%.1f"), DIAL_POSITION_INSIDE);
    else
        DrawData(bdc, 0, _T(""), _T("---"), DIAL_POSITION_INSIDE);
    if ( !std::isnan( m_PolSpd ) )
        DrawData(bdc, toUsrSpeed_Plugin(m_PolSpd, g_iDashSpeedUnit), m_StWUnit, _T("T-PS:%.1f"), DIAL_POSITION_BOTTOMLEFT);
    else
        DrawData(bdc, 0, _T(""), _T(""), DIAL_POSITION_BOTTOMLEFT);
    if ( !std::isnan( m_PolSpd_Percent ) )
        DrawData(bdc, m_PolSpd_Percent,
                 ((m_PolSpd_Percent >= POLAR_PERFORMANCE_PERCENTAGE_LIMIT) ? L"\u2191" : _T("%") ),
                 _T("%.0f"), DIAL_POSITION_BOTTOMRIGHT);
    else
        DrawData(bdc, 0, _T(""), _T(""), DIAL_POSITION_BOTTOMRIGHT);

    DrawMarkers(bdc);

	DrawLaylines(bdc);


}
/***************************************************************************************
****************************************************************************************/
void TacticsInstrument_PolarCompass::DrawBackground(wxGCDC* dc)
{
//    DrawCompassRose( dc, m_cx, m_cy, 0.7 * m_radius, m_AngleStart, true );
	DrawBoat(dc, m_cx, m_cy, m_radius);
}
/***************************************************************************************
****************************************************************************************/
void TacticsInstrument_PolarCompass::DrawBoat(wxGCDC* dc, int cx, int cy, int radius)
{
  // Now draw the boat
  wxColour cl;
  GetGlobalColor( g_sDialColorIs2, &cl );
  wxPen* pen = wxThePenList->FindOrCreatePen(cl, 1, wxPENSTYLE_SOLID);
  dc->SetPen(*pen);
  GetGlobalColor( g_sDialColorIs1, &cl );
  dc->SetBrush(cl);
  wxPoint points[7];

  /*
  *           0
  *          /\
  *         /  \
  *        /    \
  *     6 /      \ 1
  *      |        |
  *      |    X   |
  *    5 |        | 2
  *       \      /
  *        \__ _/
  *        4    3
  */
  points[0].x = cx;
  points[0].y = cy - radius * .60; // a little bit longer than compass rose
  points[1].x = cx + radius * .15;
  points[1].y = cy - radius * .08;
  points[2].x = cx + radius * .15;
  points[2].y = cy + radius * .12;
  points[3].x = cx + radius * .10;
  points[3].y = cy + radius * .40;
  points[4].x = cx - radius * .10;
  points[4].y = cy + radius * .40;
  points[5].x = cx - radius * .15;
  points[5].y = cy + radius * .12;
  points[6].x = cx - radius * .15;
  points[6].y = cy - radius * .08;

  dc->DrawPolygon(7, points, 0, 0);
  wxPoint  hdg;
  hdg.x = cx;
  hdg.y = cy - radius;
  pen->SetWidth(2);
  dc->SetPen(*pen);
  dc->DrawLine(points[0], hdg);
}

/***************************************************************************************
 Draw the 'wind needle' for  TWA.
 Just a simple line to avoid confusion with BearingCompass' placing the tip onto
 the VMG / CMG markers there.
****************************************************************************************/
void TacticsInstrument_PolarCompass::DrawWindAngles(wxGCDC* dc)
{
	if (!std::isnan(m_TWA)) {
		wxColour cl;

		GetGlobalColor(_T("BLUE3"), &cl);
		wxBrush brush;
		brush.SetStyle(wxBRUSHSTYLE_TRANSPARENT);
		brush.SetColour(cl);
		dc->SetBrush(brush);

		/* this is fix for a +/-180� round instrument, when m_MainValue is supplied as <0..180><L | R>
		* for example TWA & AWA */
		double data, TwaCog;
		// head-up = COG, but TWA is based on Hdt --> add the diff here for a correct display
		TwaCog = m_TWA; //neu, jetzt HDt 

		/* this is fix for a +/-180� round instrument, when m_MainValue is supplied as <0..180><L | R>
		* for example TWA & AWA */
		if (m_curTack == _T("\u00B0lr"))
			data = 360 - TwaCog;
		else
			data = TwaCog;

		// The arrow should stay inside fixed limits
		double val;
		if (data < m_MainValueMin)
			val = m_MainValueMin;
		else if (data > m_MainValueMax)
			val = m_MainValueMax;
		else
			val = data;

		double value = deg2rad((val - m_MainValueMin) * m_AngleRange / (m_MainValueMax - m_MainValueMin)) + deg2rad(0 - ANGLE_OFFSET);

		wxPoint brg[2];
        brg[0].x = m_cx - (m_radius *  cos(value));
        brg[0].y = m_cy - (m_radius *  sin(value));
		brg[1].x = m_cx + (m_radius * 0.7* cos(value));
		brg[1].y = m_cy + (m_radius * 0.7* sin(value));

		wxPen pen2;
        pen2.SetStyle(wxPENSTYLE_DOT);
		pen2.SetColour(cl);
		pen2.SetWidth(1);
		dc->SetPen(pen2);

		dc->DrawLine(brg[0], brg[1]);

        double value1 = deg2rad((val + 5  - m_MainValueMin) * m_AngleRange / (m_MainValueMax - m_MainValueMin)) + deg2rad(0 - ANGLE_OFFSET);
        double value2 = deg2rad((val - 5  - m_MainValueMin) * m_AngleRange / (m_MainValueMax - m_MainValueMin)) + deg2rad(0 - ANGLE_OFFSET);

        /*
        *           0
        *          /\
        *         /  \
        *        /    \
        *     2 /_ __ _\ 1
        *
        *           X
        */
        wxPoint points[4];
        points[0].x = m_cx + (m_radius * 0.7 * cos(value));
        points[0].y = m_cy + (m_radius * 0.7 * sin(value));
        points[1].x = m_cx + (m_radius * 0.8 * cos(value1));
        points[1].y = m_cy + (m_radius * 0.8 * sin(value1));
        points[2].x = m_cx + (m_radius * 0.8 * cos(value2));
        points[2].y = m_cy + (m_radius * 0.8 * sin(value2));
        dc->DrawPolygon(3, points, 0, 0);


	dc->SetPen(*wxTRANSPARENT_PEN);
	}
}
/***************************************************************************************
Draw pointers for the optimum target VMG- and CMG Angle (if bearing is available)
****************************************************************************************/
void TacticsInstrument_PolarCompass::DrawTargetxMGAngle(wxGCDC* dc)
{
    if ( !BoatPolar->isValid() )
        return;
    
    if (!std::isnan(m_TWS)) {
        // get Target VMG Angle from Polar
        TargetxMG tvmg_up = BoatPolar->GetTargetVMGUpwind(m_TWS);
        TargetxMG tvmg_dn = BoatPolar->GetTargetVMGDownwind(m_TWS);
        TargetxMG TCMGMax;
        TargetxMG TCMGMin;

        if (tvmg_up.TargetAngle > 0){
            DrawTargetAngle(dc, tvmg_up.TargetAngle, _T("BLUE3"), 2);
            DrawTargetAngle(dc, 360-tvmg_up.TargetAngle, _T("BLUE3"), 2);
        }
        if (tvmg_dn.TargetAngle > 0) {
            DrawTargetAngle(dc, tvmg_dn.TargetAngle, _T("BLUE3"), 2);
            DrawTargetAngle(dc, 360-tvmg_dn.TargetAngle, _T("BLUE3"), 2);
        }
        if (!std::isnan(m_Bearing)){
            if (m_Bearing >= 0 && m_Bearing < 360 && !std::isnan(m_TWD)){
                BoatPolar->Calc_TargetCMG2(m_TWS, m_TWD, m_Bearing, &TCMGMax, &TCMGMin);
                if (!std::isnan(TCMGMax.TargetAngle))      DrawTargetAngle(dc, TCMGMax.TargetAngle, _T("URED"), 2);
                if (!std::isnan(TCMGMin.TargetAngle))      DrawTargetAngle(dc, TCMGMin.TargetAngle, _T("URED"), 1);
            }
        }
    }
}
/***************************************************************************************
Draw pointers for the optimum target VMG- and CMG Angle (if bearing is available)
****************************************************************************************/
void TacticsInstrument_PolarCompass::DrawTargetAngle(wxGCDC* dc, double TargetAngle, wxString color, int size){
  if (TargetAngle > 0 && !std::isnan(m_Hdt) && !std::isnan(m_TWD)){
      wxColour cl;
      dc->SetPen(*wxTRANSPARENT_PEN);
      GetGlobalColor(color, &cl);
      wxBrush brush;
      brush.SetStyle(wxBRUSHSTYLE_SOLID);
      brush.SetColour(cl);
      dc->SetBrush(brush);

       double data, TwaCog;
       double rotangle = getSignedDegRange(m_Hdt, m_TWD);

      // head-up = COG, but TWA is based on Hdt --> add the diff here for a correct display
       TwaCog = rotangle+TargetAngle;
       if (TwaCog > 360) TwaCog -= 360;
       if (TwaCog <0 ) TwaCog += 360;

         data = TwaCog;

      // The arrow should stay inside fixed limits
      double val;
      if (data < m_MainValueMin)
        val = m_MainValueMin;
      else if (data > m_MainValueMax)
        val = m_MainValueMax;
      else
        val = data;
      double sizefactor, widthfactor;
      if (size == 1) {
        sizefactor = 0.935;
        widthfactor = 0.90;
      }
      else{
        sizefactor = 1;
        widthfactor = 1;
      }

      double value = deg2rad((val - m_MainValueMin) * m_AngleRange / (m_MainValueMax - m_MainValueMin)) + deg2rad(0 - ANGLE_OFFSET);
      double value1 = deg2rad((val + 5*widthfactor - m_MainValueMin) * m_AngleRange / (m_MainValueMax - m_MainValueMin)) + deg2rad(0 - ANGLE_OFFSET);
      double value2 = deg2rad((val - 5 * widthfactor - m_MainValueMin) * m_AngleRange / (m_MainValueMax - m_MainValueMin)) + deg2rad(0 - ANGLE_OFFSET);

      /*
      *           0
      *          /\
      *         /  \
      *        /    \
      *     2 /_ __ _\ 1
      *
      *           X
      */
      wxPoint points[4];
      points[0].x = m_cx + (m_radius * 0.95 * cos(value));
      points[0].y = m_cy + (m_radius * 0.95 * sin(value));
      points[1].x = m_cx + (m_radius * 1.15*sizefactor * cos(value1));
      points[1].y = m_cy + (m_radius * 1.15*sizefactor * sin(value1));
      points[2].x = m_cx + (m_radius * 1.15*sizefactor * cos(value2));
      points[2].y = m_cy + (m_radius * 1.15*sizefactor * sin(value2));
      dc->DrawPolygon(3, points, 0, 0);
    //}
  }
}
/***************************************************************************************
****************************************************************************************/
void TacticsInstrument_PolarCompass::DrawForeground(wxGCDC* dc)
{
  if (!std::isnan(m_Bearing))
		DrawBearing(dc);
    if (!std::isnan(m_TWS) && !std::isnan(m_TWA)) {
      DrawPolar(dc);
      DrawWindAngles(dc);
      DrawTargetxMGAngle(dc);
    }
}
/***************************************************************************************
****************************************************************************************/
void TacticsInstrument_PolarCompass::DrawBearing(wxGCDC* dc)
{
	wxColour cl;
	// no border for the circle 
	dc->SetPen(*wxTRANSPARENT_PEN);

	GetGlobalColor(_T("URED"), &cl);
	wxBrush brush;
	brush.SetStyle(wxBRUSHSTYLE_SOLID);
	brush.SetColour(cl);
	dc->SetBrush(brush);

	double value = deg2rad(m_Bearing) + deg2rad(m_AngleStart - ANGLE_OFFSET);

	dc->DrawCircle(m_cx + (m_radius * 0.8 * cos(value)), m_cy + (m_radius * 0.75 * sin(value)), m_radius / 16);
	wxPoint brg[2];
	brg[0].x = m_cx + (m_radius * 0.8 * cos(value));
	brg[0].y = m_cy + (m_radius * 0.8 * sin(value));
	brg[1].x = m_cx + (m_radius * 0.98 * cos(value));
	brg[1].y = m_cy + (m_radius * 0.98 * sin(value));

	wxPen pen2;
	pen2.SetStyle(wxPENSTYLE_SOLID);
	pen2.SetColour(cl);
	pen2.SetWidth(2);
	dc->SetPen(pen2);

	dc->DrawLine(brg[0],brg[1]);
	dc->SetPen(*wxTRANSPARENT_PEN);

}
/***************************************************************************************
****************************************************************************************/
#define POLSTEPS 180 //we draw in 2 degree steps
void TacticsInstrument_PolarCompass::DrawPolar(wxGCDC*dc)
{
    if ( !BoatPolar->isValid() )
        return;
    
    if (!std::isnan(m_TWS) && !std::isnan(m_TWD)) {
        wxColour cl;
        GetGlobalColor(_T("UBLCK"), &cl);
        wxPen pen1;
        pen1.SetStyle(wxPENSTYLE_SOLID);
        pen1.SetColour(cl);
        pen1.SetWidth(1);
        dc->SetPen(pen1);
        double polval[POLSTEPS];
        double max = 0;
        int i;
        for (i = 0; i < POLSTEPS / 2; i++){ //0...179
            //wxLogMessage("-- ..PolarCompass-DrawPolar() - i=%d m_TWS=%f", i, m_TWS);
            polval[i] = BoatPolar->GetPolarSpeed(i*2 + 1, m_TWS); //polar data is 1...180 !!! i*2 : we draw in 2 degree steps
            polval[POLSTEPS - 1 - i] = polval[i];
            if (std::isnan(polval[i]))polval[i] = polval[POLSTEPS - 1 - i] = 0.0;
            if (polval[i]>max) max = polval[i];
        }
        wxPoint currpoints[POLSTEPS];
        for ( i = 0; i < POLSTEPS; i++){
            double anglevalue = deg2rad(m_TWD + i*2) + deg2rad(m_AngleStart - ANGLE_OFFSET);
            double rad = m_radius*0.69*polval[i] / max;
            currpoints[i].x = m_cx + (rad * cos(anglevalue));
            currpoints[i].y = m_cy + (rad * sin(anglevalue));
        }
        wxBrush currbrush;
        currbrush.SetColour(wxColour(7, 107, 183, 0));
        currbrush.SetStyle(wxBRUSHSTYLE_SOLID);
        dc->SetBrush(currbrush);
        dc->DrawPolygon(POLSTEPS, currpoints, 0, 0);
    }
}

/***************************************************************************************
****************************************************************************************/
void TacticsInstrument_PolarCompass::DrawData(wxGCDC* dc, double value,
	wxString unit, wxString format, DialPositionOption position)
{
	if (position == DIAL_POSITION_NONE)
		return;

	dc->SetFont(*g_pFontLabel);
	wxColour cl;
	GetGlobalColor( g_sDialColorForeground, &cl );
	dc->SetTextForeground(cl);

	wxSize size = GetClientSize();

	wxString text;
	if (!std::isnan(value))
	{
		if (unit == _T("\u00B0"))
			text = wxString::Format(format, value) + DEGREE_SIGN;
		else if (unit == _T("\u00B0lr")) // No special display for now, might be XX�< (as in text-only instrument) - no support for B0R
			text = wxString::Format(format, value) + DEGREE_SIGN;
		else if (unit == _T("\u00B0rl")) // No special display for now, might be >XX� - no support for B0L
			text = wxString::Format(format, value) + DEGREE_SIGN;
		else if (unit == _T("\u00B0T"))
			text = wxString::Format(format, value) + DEGREE_SIGN + _T("T");
		else if (unit == _T("\u00B0M"))
			text = wxString::Format(format, value) + DEGREE_SIGN + _T("M");
		else if (unit == _T("N")) // Knots
			text = wxString::Format(format, value) + _T(" Kts");
		else
			text = wxString::Format(format, value) + _T(" ") + unit;
	}
	else
		text = _T("---");

	int width, height;
	dc->GetMultiLineTextExtent(text, &width, &height, NULL, g_pFontLabel);

	wxRect TextPoint;
	TextPoint.width = width;
	TextPoint.height = height;
	wxColour c3;

	switch (position)
	{
	case DIAL_POSITION_NONE:
		GetGlobalColor( g_sDialColorForeground, &c3 );
		// This case was already handled before, it's here just
		// to avoid compiler warning.
		return;
	case DIAL_POSITION_INSIDE:
	{
		GetGlobalColor(_T("BLUE2"), &c3);
		TextPoint.x = m_cx - (width / 2) - 1;
		TextPoint.y = (size.y * .75) - height;
		GetGlobalColor( g_sDialColorLabel, &cl );
        int penwidth = 1;//size.x / 100;
		wxPen* pen = wxThePenList->FindOrCreatePen(cl, penwidth, wxPENSTYLE_SOLID);
		dc->SetPen(*pen);
		GetGlobalColor( g_sDialColorBackground, &cl );
		dc->SetBrush(cl);
		// There might be a background drawn below
		// so we must clear it first.
		dc->DrawRoundedRectangle(TextPoint.x - 2, TextPoint.y - 2, width + 4, height + 4, 3);
		break;
	}
	case DIAL_POSITION_TOPINSIDE:
	{
		GetGlobalColor( g_sDialColorForeground, &c3 );
		TextPoint.x = m_cx - (width / 2) - 1;
		TextPoint.y = m_cy- 0.95*m_radius - height;
		GetGlobalColor(_T("UBLCK"), &cl);
		int penwidth = size.x / 100;
		wxPen* pen = wxThePenList->FindOrCreatePen(cl, penwidth, wxPENSTYLE_SOLID);
		dc->SetPen(*pen);
		GetGlobalColor( g_sDialColorBackground, &cl );
		dc->SetBrush(cl);
		// There might be a background drawn below
		// so we must clear it first.
		dc->DrawRoundedRectangle(TextPoint.x - 2, TextPoint.y - 2, width + 4, height + 4, 3);
		break;
	}
	case DIAL_POSITION_TOPLEFT:
		GetGlobalColor( g_sDialColorForeground, &c3 );
		TextPoint.x = 0;
		TextPoint.y = m_TitleHeight;
		break;
	case DIAL_POSITION_TOPRIGHT:
		GetGlobalColor( g_sDialColorForeground, &c3 );
		TextPoint.x = size.x - width - 1;
		TextPoint.y = m_TitleHeight;
		break;
	case DIAL_POSITION_BOTTOMLEFT:
		GetGlobalColor( g_sDialColorForeground, &c3 );
		TextPoint.x = 0;
		TextPoint.y = size.y - height;
		break;
	case DIAL_POSITION_BOTTOMRIGHT:
		GetGlobalColor( g_sDialColorForeground, &c3 );
		TextPoint.x = size.x - width - 1;
		TextPoint.y = size.y - height;
		break;
	}

	wxColour c2;
	GetGlobalColor( g_sDialColorBackground, &c2 );

	wxStringTokenizer tkz(text, _T("\n"));
	wxString token;

	token = tkz.GetNextToken();
	while (token.Length()) {
		dc->GetTextExtent(token, &width, &height, NULL, NULL, g_pFontLabel);

#ifdef __WXMSW__
		if (g_pFontLabel->GetPointSize() <= 12) {
			wxBitmap tbm(width, height, -1);
			wxMemoryDC tdc(tbm);

			tdc.SetBackground(c2);
			tdc.Clear();
			tdc.SetFont(*g_pFontLabel);
			tdc.SetTextForeground(c3);

			tdc.DrawText(token, 0, 0);
			tdc.SelectObject(wxNullBitmap);

			dc->DrawBitmap(tbm, TextPoint.x, TextPoint.y, false);
		}
		else
#endif
			dc->DrawText(token, TextPoint.x, TextPoint.y);


		TextPoint.y += height;
		token = tkz.GetNextToken();
	}
}
/***************************************************************************************
  Calculate & Draw the laylines for the bearing compass 
****************************************************************************************/
void TacticsInstrument_PolarCompass::DrawLaylines(wxGCDC* dc)
{
  if (!std::isnan(m_Cog) && !std::isnan(m_Hdt) && !std::isnan(m_lat) && !std::isnan(m_lon) && !std::isnan(m_TWA) && !std::isnan(m_CurrDir) && !std::isnan(m_CurrSpeed)){


		wxColour cl;
		GetGlobalColor( g_sDialColorIs2, &cl );
		wxPen pen1;
		pen1.SetStyle(wxPENSTYLE_SOLID);
		pen1.SetColour(cl);
		pen1.SetWidth(2);
		dc->SetPen(pen1);
		GetGlobalColor( g_sDialColorIs1, &cl );
		wxBrush brush1;
		brush1.SetStyle(wxBRUSHSTYLE_SOLID);
		brush1.SetColour(cl);
		dc->SetBrush(brush1);

		dc->SetPen(*wxTRANSPARENT_PEN);

		GetGlobalColor( g_sDialSecondNeedleColor, &cl );
		wxBrush vbrush, tackbrush;
		vbrush.SetStyle(wxBRUSHSTYLE_SOLID);
		tackbrush.SetStyle(wxBRUSHSTYLE_SOLID);
		//m_curTack = TWA unit
		//it shows L= wind from left = port tack or R=wind from right = starboard tack
		//we're on port tack, so vertical layline is red
		if (m_curTack == _T("\u00B0lr")) {
			vbrush.SetColour(wxColour(204, 41, 41, 128));  //red, transparent
			tackbrush.SetColour(wxColour(0, 200, 0, 128)); //green, transparent
			m_targetTack = _("R");
		}
		else if (m_curTack == _T("\u00B0rl"))  {// we're on starboard tack, so vertical layline is green
			vbrush.SetColour(wxColour(0, 200, 0, 128));      //green, transparent
			tackbrush.SetColour(wxColour(204, 41, 41, 128)); //red, transparent
			m_targetTack = _("L");
		}
		double value1 = deg2rad(m_Cog - m_ExpSmoothDegRange / 2. ) + deg2rad(m_AngleStart - ANGLE_OFFSET);
		double value2 = deg2rad(m_Cog + m_ExpSmoothDegRange / 2. ) + deg2rad(m_AngleStart - ANGLE_OFFSET);

		//draw the vertical layline
		dc->SetBrush(vbrush);
		wxPoint vpoints[3];
		vpoints[0].x = m_cx;
		vpoints[0].y = m_cy;
		vpoints[1].x = m_cx + (m_radius * cos(value1));//neu : f�r Headup = HDT
		vpoints[1].y = m_cy + (m_radius * sin(value1));//neu : f�r Headup = HDT
		vpoints[2].x = m_cx + (m_radius * cos(value2));//neu : f�r Headup = HDT
		vpoints[2].y = m_cy + (m_radius * sin(value2));//neu : f�r Headup = HDT
		dc->DrawArc(vpoints[2], vpoints[1], vpoints[0]);

		/*****************************************************************************************
		Caclulate and draw  the second layline (for other tack) :
		---------------------------------------------------------
		Approach : in the bearing compass display, "head up" = COG. And TWA is based on boat heading (Hdt).
		to calculate the layline of the other tack, sum up
		diff_btw_Cog_and_HDG(now we're on Hdt) ; this is NOT NEEDED if Head-Up = Hdt !!! 
		+ 2 x TWA
		+ Leeway
		---------
		= predictedHdt
		+ current_angle
		======================
		= newCog (on other tack)
		Calculation of (sea) current angle :
		1. from actual pos. calculate the endpoint of predictedHdt (out: predictedLatHdt, predictedLonHdt),
		assuming same StW on other tack
		2. at that point apply current : startpoint predictedLatHdt, predictedLonHdt + bearing + speed; out : predictedLatCog, predictedLonCog
		3. calculate angle (and speed) from curr pos to predictedLatCog, predictedLonCog; out : newCog + newSOG
		********************************************************************************************/
		dc->SetBrush(tackbrush);
        double predictedKdW; //==predicted Course Through Water
        double Leeway = (m_LeewayUnit == _T("\u00B0lr")) ? -m_Leeway : m_Leeway;
        if (m_targetTack == _T("R")){ // so currently wind is from port ...
			//predictedKdW = m_Hdt - 2 * m_TWA - m_Leeway;
          predictedKdW = m_Hdt - 2 * m_TWA - Leeway;
        }
		else if (m_targetTack == _T("L")){ //so, currently wind from starboard
			//predictedKdW = m_Hdt + 2 * m_TWA + m_Leeway;
          predictedKdW = m_Hdt + 2 * m_TWA - Leeway;
        }
		else {
          predictedKdW = (m_TWA < 10) ? 180 : 0; // should never happen, but is this correct ???
		}
        if (predictedKdW >= 360) predictedKdW -= 360;
        if (predictedKdW < 0) predictedKdW += 360;
        double predictedLatHdt, predictedLonHdt, predictedLatCog, predictedLonCog;
		double predictedCoG;
        //standard triangle calculation to get predicted CoG / SoG
        //get endpoint from boat-position by applying  KdW, StW
        PositionBearingDistanceMercator_Plugin(m_lat, m_lon, predictedKdW, fromUsrSpeed_Plugin(m_StW, g_iDashSpeedUnit), &predictedLatHdt, &predictedLonHdt);
        //apply surface current with direction & speed to endpoint from above
		PositionBearingDistanceMercator_Plugin(predictedLatHdt, predictedLonHdt, m_CurrDir, m_CurrSpeed, &predictedLatCog, &predictedLonCog);
		//now get predicted CoG & SoG as difference between the 2 endpoints (coordinates) from above
        DistanceBearingMercator_Plugin(predictedLatCog, predictedLonCog, m_lat, m_lon, &predictedCoG, &m_predictedSog);

		value1 = deg2rad(predictedCoG - m_ExpSmoothDegRange / 2.) + deg2rad(m_AngleStart - ANGLE_OFFSET);
		value2 = deg2rad(predictedCoG + m_ExpSmoothDegRange / 2.) + deg2rad(m_AngleStart - ANGLE_OFFSET);

		wxPoint tackpoints[3];
		tackpoints[0].x = m_cx;
		tackpoints[0].y = m_cy;
		tackpoints[1].x = m_cx + (m_radius * cos(value1));
		tackpoints[1].y = m_cy + (m_radius * sin(value1));
		tackpoints[2].x = m_cx + (m_radius * cos(value2));
		tackpoints[2].y = m_cy + (m_radius * sin(value2));
		dc->DrawArc(tackpoints[2], tackpoints[1], tackpoints[0]);
	}
}
/***************************************************************************************
Calculates the width of the laylines.
This depends on the boat's yawing around COG and should give you a hint on the accuracy
of the predicted layline
****************************************************************************************/
void TacticsInstrument_PolarCompass::CalculateLaylineDegreeRange(void)
{
	//calculate degree-range for laylines
	//do some exponential smoothing on degree range of COGs
	if (!std::isnan(m_Cog) && m_Cog != m_COGRange[0]){
		double mincog = 360, maxcog = 0;
		for (int i = 0; i < COGRANGE; i++){
			if (m_COGRange[i] < mincog) mincog = m_COGRange[i];
			if (m_COGRange[i] > maxcog) maxcog = m_COGRange[i];
		}
         m_LaylineDegRange = getDegRange(maxcog, mincog);

		for (int i = 0; i < COGRANGE - 1; i++) m_COGRange[i + 1] = m_COGRange[i];
		m_COGRange[0] = m_Cog;
		if (m_LaylineDegRange < g_iMinLaylineWidth){
			m_LaylineDegRange = g_iMinLaylineWidth;
		}
		else if (m_LaylineDegRange > g_iMaxLaylineWidth){
			m_LaylineDegRange = g_iMaxLaylineWidth;
		}
		mExpSmDegRange->SetAlpha(g_dalphaDeltCoG);
		m_ExpSmoothDegRange = mExpSmDegRange->GetSmoothVal(m_LaylineDegRange);


	}

}
/***************************************************************************************
****************************************************************************************/
bool TacticsInstrument_PolarCompass::LoadConfig(void)
{
    // port remark: placeholder only, config in globals
    return true;
}
/***************************************************************************************
****************************************************************************************/
bool TacticsInstrument_PolarCompass::SaveConfig(void)
{
    // port remark: placeholder only, config saved from globals
    return true;
}
