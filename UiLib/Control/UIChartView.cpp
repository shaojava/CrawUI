#include "StdAfx.h"
#include "UIChartView.h"

namespace UiLib
{
	REGIST_DUICLASS(CChartViewUI);
	//************************************
	// 函数名称: CChartSeries
	// 返回类型: 
	// 参数信息: LPCTSTR _LegendText
	// 参数信息: LPCTSTR _SeriesColorA
	// 参数信息: LPCTSTR _SeriesColorB
	// 函数说明: 
	//************************************
	CChartSeries::CChartSeries( LPCTSTR _LegendText,LPCTSTR _SeriesColorA,LPCTSTR _SeriesColorB /*= NULL*/ ) : 
		m_bIsVisible(true),			//是否显示当前组数据报表图例
		m_uSeriesFondId(0),			//图例字体ID
		m_dSeriesColorA(0),			//数据图例颜色，渐变开始色
		m_dSeriesColorB(0),			//数据图例颜色，渐变结束色。饼图将忽略该值
		m_pChartView(NULL)			//图表指针
	{
		m_sLegendText	= _LegendText;

		SetSeriesColor(_SeriesColorA,_SeriesColorB);
	}

	//************************************
	// 函数名称: CChartSeries
	// 返回类型: 
	// 参数信息: LPCTSTR _LegendText
	// 参数信息: DWORD _SeriesColorA
	// 参数信息: DWORD _SeriesColorB
	// 函数说明: 
	//************************************
	CChartSeries::CChartSeries( LPCTSTR _LegendText,DWORD _SeriesColorA,DWORD _SeriesColorB ) : 
		m_bIsVisible(true),			//是否显示当前组数据报表图例
		m_uSeriesFondId(0),			//图例字体ID
		m_dSeriesColorA(0),			//数据图例颜色，渐变开始色
		m_dSeriesColorB(0),			//数据图例颜色，渐变结束色。饼图将忽略该值
		m_pChartView(NULL)			//图表指针
	{
		m_sLegendText	= _LegendText;

		SetSeriesColor(_SeriesColorA,_SeriesColorB);
	}

	//************************************
	// 函数名称: ~CChartSeries
	// 返回类型: 
	// 参数信息: void
	// 函数说明: 
	//************************************
	CChartSeries::~CChartSeries( void )
	{
		RemoveSeriesDataAll();
	}

	//************************************
	// 函数名称: SetLegendText
	// 返回类型: void
	// 参数信息: LPCTSTR _LegendText
	// 函数说明: 
	//************************************
	void CChartSeries::SetLegendText( LPCTSTR _LegendText )
	{
		m_sLegendText = _LegendText;

		if(m_pChartView)
			m_pChartView->CalLegendSize(this);
		Invalidate();
	}

	//************************************
	// 函数名称: GetLegendText
	// 返回类型: LPCTSTR
	// 函数说明: 
	//************************************
	LPCTSTR CChartSeries::GetLegendText()
	{
		return m_sLegendText.GetData();
	}

	//************************************
	// 函数名称: SetSeriesColor
	// 返回类型: void
	// 参数信息: LPCTSTR _SeriesColorA
	// 参数信息: LPCTSTR _SeriesColorB
	// 函数说明: 
	//************************************
	void CChartSeries::SetSeriesColor( LPCTSTR _SeriesColorA,LPCTSTR _SeriesColorB )
	{
		if(_SeriesColorA){
			if( *_SeriesColorA == _T('#'))
				_SeriesColorA = ::CharNext(_SeriesColorA);
			LPTSTR pstr = NULL;
			m_dSeriesColorA = _tcstoul(_SeriesColorA, &pstr, 16);
		}

		if(_SeriesColorB){
			if( *_SeriesColorB == _T('#'))
				_SeriesColorB = ::CharNext(_SeriesColorB);
			LPTSTR pstr = NULL;
			m_dSeriesColorB = _tcstoul(_SeriesColorB, &pstr, 16);
		}
		Invalidate();
	}

	//************************************
	// 函数名称: SetSeriesColor
	// 返回类型: void
	// 参数信息: DWORD _SeriesColorA
	// 参数信息: DWORD _SeriesColorB
	// 函数说明: 
	//************************************
	void CChartSeries::SetSeriesColor( DWORD _SeriesColorA,DWORD _SeriesColorB )
	{
		m_dSeriesColorA = _SeriesColorA;
		m_dSeriesColorB	= _SeriesColorB;
		Invalidate();
	}

	//************************************
	// 函数名称: GetSeriesColorA
	// 返回类型: DWORD
	// 函数说明: 
	//************************************
	DWORD CChartSeries::GetSeriesColorA()
	{
		return m_dSeriesColorA;
	}

	//************************************
	// 函数名称: GetSeriesColorB
	// 返回类型: DWORD
	// 函数说明: 
	//************************************
	DWORD CChartSeries::GetSeriesColorB()
	{
		return m_dSeriesColorB;
	}

	//************************************
	// 函数名称: AddSeriesData
	// 返回类型: bool
	// 参数信息: CChartItem * _ChartItem
	// 函数说明: 
	//************************************
	bool CChartSeries::AddSeriesData( CChartItem* _ChartItem )
	{
		bool nRet = m_rSeriesData.Add(_ChartItem);
		Invalidate();
		return nRet;
	}

	//************************************
	// 函数名称: AddSeriesData
	// 返回类型: bool
	// 参数信息: double _DataValue
	// 函数说明: 
	//************************************
	bool CChartSeries::AddSeriesData( double _DataValue )
	{
		bool nRet = m_rSeriesData.Add(new CChartItem(_DataValue));
		Invalidate();
		return nRet;
	}

	//************************************
	// 函数名称: SetSeriesData
	// 返回类型: bool
	// 参数信息: int _Index
	// 参数信息: CChartItem * _ChartItem
	// 函数说明: 
	//************************************
	bool CChartSeries::SetSeriesData( int _Index,CChartItem* _ChartItem )
	{
		bool nRet = m_rSeriesData.SetAt(_Index,_ChartItem);
		Invalidate();
		return nRet;
	}

	//************************************
	// 函数名称: SetSeriesData
	// 返回类型: bool
	// 参数信息: int _Index
	// 参数信息: double _DataValue
	// 函数说明: 
	//************************************
	bool CChartSeries::SetSeriesData( int _Index,double _DataValue )
	{
		bool nRet = m_rSeriesData.SetAt(_Index,new CChartItem(_DataValue));
		Invalidate();
		return nRet;
	}

	//************************************
	// 函数名称: InsertSeriesData
	// 返回类型: bool
	// 参数信息: int _Index
	// 参数信息: CChartItem * _ChartItem
	// 函数说明: 
	//************************************
	bool CChartSeries::InsertSeriesData( int _Index,CChartItem* _ChartItem )
	{
		bool nRet = m_rSeriesData.InsertAt(_Index,_ChartItem);
		Invalidate();
		return nRet;
	}

	//************************************
	// 函数名称: InsertSeriesData
	// 返回类型: bool
	// 参数信息: int _Index
	// 参数信息: double _DataValue
	// 函数说明: 
	//************************************
	bool CChartSeries::InsertSeriesData( int _Index,double _DataValue )
	{
		bool nRet = m_rSeriesData.InsertAt(_Index,new CChartItem(_DataValue));
		Invalidate();
		return nRet;
	}

	//************************************
	// 函数名称: RemoveSeriesDataAt
	// 返回类型: bool
	// 参数信息: int _Index
	// 函数说明: 
	//************************************
	bool CChartSeries::RemoveSeriesDataAt( int _Index )
	{
		CChartItem* pItem = m_rSeriesData.GetAt(_Index);
		if(pItem && m_rSeriesData.Remove(_Index)){
			delete pItem;
			pItem = NULL;

			Invalidate();
			return true;
		}
		return false;
	}

	//************************************
	// 函数名称: RemoveSeriesDataAll
	// 返回类型: void
	// 函数说明: 
	//************************************
	void CChartSeries::RemoveSeriesDataAll()
	{
		int nCount =  m_rSeriesData.GetSize();
		for(int iIndex = 0;iIndex < nCount;iIndex++)
		{
			CChartItem* pItem = m_rSeriesData.GetAt(iIndex);
			if(pItem)
			{
				delete pItem;
				pItem = NULL;
			}
		}
		for(int i = 0 ; i < nCount ; i++)
			m_rSeriesData.Remove(i);
		m_rSeriesData.Resize(0);
		Invalidate();
	}

	//************************************
	// 函数名称: GetSeriesData
	// 返回类型: CChartItem*
	// 参数信息: int _Index
	// 函数说明: 
	//************************************
	CChartItem* CChartSeries::GetSeriesData( int _Index )
	{
		return m_rSeriesData.GetAt(_Index);
	}
	
	//************************************
	// 函数名称: GetDataCount
	// 返回类型: int
	// 函数说明: 
	//************************************
	int CChartSeries::GetDataCount()
	{
		return m_rSeriesData.GetSize();
	}

	//************************************
	// 函数名称: SetVisible
	// 返回类型: void
	// 参数信息: bool _Visible
	// 函数说明: 
	//************************************
	void CChartSeries::SetVisible( bool _Visible )
	{
		m_bIsVisible = _Visible;
		Invalidate();
	}

	//************************************
	// 函数名称: GetVisible
	// 返回类型: bool
	// 函数说明: 
	//************************************
	bool CChartSeries::GetVisible()
	{
		return m_bIsVisible;
	}

	//************************************
	// 函数名称: SetChartViewObj
	// 返回类型: void
	// 参数信息: CChartViewUI * _pChartViewUI
	// 函数说明: 
	//************************************
	void CChartSeries::SetChartViewObj( CChartViewUI* _pChartViewUI )
	{
		m_pChartView = _pChartViewUI;
	}

	//************************************
	// 函数名称: Invalidate
	// 返回类型: void
	// 函数说明: 
	//************************************
	void CChartSeries::Invalidate()
	{
		if(m_pChartView)
			m_pChartView->Invalidate();
	}

	/**************************************************************************************************************************************************************************************************/
	/*
	/**************************************************************************************************************************************************************************************************/

	//************************************
	// 函数名称: CChartXYAxis
	// 返回类型: 
	// 参数信息: void
	// 函数说明: 
	//************************************
	CChartXYAxis::CChartXYAxis( void ) :
		m_bVisibelCustomTick(true),			//是否显示自定义刻度
		m_bVisibleBaseLine(true),			//是否显示基线
		m_iMinTick(0),						//刻度最小值
		m_iMaxTick(5),						//刻度最大值
		m_iMinTickEx(0),					//附加刻度最小值
		m_iMaxTickEx(5),					//附加刻度最大值
		m_uStepTick(1),						//刻度间隔
		m_iMaxGraphWidth(0),				//图表绘图区域最大宽度
		m_iMaxGraphHeight(0),				//图表绘图区域最大高度
		m_uMaxAxisLabelWidth(0),			//图表数据组标签最大宽度
		m_uMaxAxisLabelHeight(16),			//图表数据组标签最大高度
		m_uMaxDataLabelWidth(25),			//图表数据刻度最大宽
		m_uLegendLocation(LOCATION_BOTTOM),	//图例位置
		m_uLabelLocation(LOCATION_BOTTOM),	//分组标签刻度位置
		m_uAlignment(CHART_HORIZONTAL),		//图表方向。饼图将忽略该值
		m_uXYAxisFontId(0),					//刻度文字字体ID
		m_uXYAxisLineSize(1),				//刻度线条大小
		m_uLegendMaxWidth(60),				//图例面板最大宽度
		m_uLegendMaxHeight(22),				//图例面板最大高度
		m_dDataScale(0.0),					//一个数据单位所占用的像素
		m_iAxissCale(0),					//基线位置
		m_iMaxBaseLine(0),					//基线最大值
		m_iMinBaseLine(0),					//基线最小值
		m_dXYAxisLineColor(0xFF000000),		//刻度线条颜色
		m_pChartView(NULL)
	{
		m_sLegendTitel			= _T("Legend");
		m_rcChartPadding.left	= 5;
		m_rcChartPadding.top	= 5;
		m_rcChartPadding.right	= 5;
		m_rcChartPadding.bottom	= 5;
		SetTickLimis(GetMinTick(),GetMaxTick(),GetSetpTick());
	}

	//************************************
	// 函数名称: ~CChartXYAxis
	// 返回类型: 
	// 参数信息: void
	// 函数说明: 
	//************************************
	CChartXYAxis::~CChartXYAxis( void )
	{

	}

	//************************************
	// 函数名称: SetTickLimis
	// 返回类型: void
	// 参数信息: int _MinTick
	// 参数信息: int _MaxTick
	// 参数信息: UINT _StepTick
	// 函数说明: 
	//************************************
	void CChartXYAxis::SetTickLimis( int _MinTick,int _MaxTick,UINT _StepTick )
	{
		m_iMinTick	= _MinTick<GetMinTick()?_MinTick:m_iMinTick;
		m_iMaxTick	= _MaxTick>GetMaxTick()?_MaxTick:m_iMaxTick;
		m_uStepTick	= (_StepTick<=0||_StepTick < GetSetpTick())?(int)((m_iMaxTick - m_iMinTick)/5):_StepTick;
		m_rDataAxis.Empty();

		if(_StepTick){
			m_rDataAxis.Add(0);
			for(int nIndex = _StepTick;nIndex < _MaxTick;nIndex += _StepTick){
				m_rDataAxis.Add(nIndex);
			}
			for(int nIndex = 0-_StepTick;nIndex > _MinTick;nIndex -= _StepTick){
				m_rDataAxis.Add(nIndex);
			}
			m_rDataAxis.Add(_MinTick);
			m_rDataAxis.Add(_MaxTick);
		}

		Invalidate();
	}

	//************************************
	// 函数名称: SetMinTick
	// 返回类型: void
	// 参数信息: int _MinTick
	// 函数说明: 
	//************************************
	void CChartXYAxis::SetMinTick( int _MinTick )
	{
		SetTickLimis(_MinTick,GetMaxTick(),GetSetpTick());
	}

	//************************************
	// 函数名称: SetMaxTick
	// 返回类型: void
	// 参数信息: int _MaxTick
	// 函数说明: 
	//************************************
	void CChartXYAxis::SetMaxTick( int _MaxTick )
	{
		SetTickLimis(GetMinTick(),_MaxTick,GetSetpTick());
	}

	//************************************
	// 函数名称: SetStepTick
	// 返回类型: void
	// 参数信息: int _StepTick
	// 函数说明：#end$
	//************************************
	void CChartXYAxis::SetStepTick( int _StepTick /*= 0*/ )
	{
		SetTickLimis(GetMinTick(),GetMaxTick(),_StepTick);
	}

	//************************************
	// 函数名称: GetMinTick
	// 返回类型: int
	// 函数说明: 
	//************************************
	int CChartXYAxis::GetMinTick()
	{
		return m_iMinTick < m_iMinTickEx?m_iMinTick:m_iMinTickEx;
	}

	//************************************
	// 函数名称: GetMaxTick
	// 返回类型: int
	// 函数说明: 
	//************************************
	int CChartXYAxis::GetMaxTick()
	{
		return m_iMaxTick > m_iMaxTickEx?m_iMaxTick:m_iMaxTickEx;
	}

	//************************************
	// 函数名称: GetSetpTick
	// 返回类型: UINT
	// 函数说明: 
	//************************************
	UINT CChartXYAxis::GetSetpTick()
	{
		return m_uStepTick;
	}
	
	//************************************
	// 函数名称: ResetTick
	// 返回类型: void
	// 函数说明：#end$
	//************************************
	void CChartXYAxis::ResetTick()
	{
		m_iMaxTick		= 0;
		m_iMaxTickEx	= 5;
		m_iMinTick		= 0;
		m_iMinTickEx	= 5;
		m_uStepTick		= 1;

		SetTickLimis(GetMinTick(),GetMaxTick(),GetSetpTick());
	}

	//************************************
	// 函数名称: AddLabel
	// 返回类型: void
	// 参数信息: LPCTSTR _DataLabel
	// 函数说明: 
	//************************************
	void CChartXYAxis::AddLabel( LPCTSTR _DataLabel )
	{
		m_rChartLabels.Add(_DataLabel);
		Invalidate();
	}

	//************************************
	// 函数名称: AddCustomTick
	// 返回类型: void
	// 参数信息: int _TickValue
	// 函数说明: 
	//************************************
	void CChartXYAxis::AddCustomTick( int _TickValue )
	{
		m_rDataCustomAxis.Add(_TickValue);
		if(_TickValue > m_iMaxTickEx)
			m_iMaxTickEx = _TickValue;
		if(_TickValue < m_iMinTickEx)
			m_iMinTickEx = _TickValue;

		Invalidate();
	}

	//************************************
	// 函数名称: SetVisibleCoustomTick
	// 返回类型: void
	// 参数信息: bool _Visible
	// 函数说明: 
	//************************************
	void CChartXYAxis::SetVisibleCoustomTick( bool _Visible )
	{
		m_bVisibelCustomTick = _Visible;
		Invalidate();
	}

	//************************************
	// 函数名称: GetVisibleCoustomTick
	// 返回类型: bool
	// 函数说明: 
	//************************************
	bool CChartXYAxis::GetVisibleCoustomTick()
	{
		return m_bVisibelCustomTick;
	}

	//************************************
	// 函数名称: SetXYAxisFontId
	// 返回类型: void
	// 参数信息: int _FontId
	// 函数说明: 
	//************************************
	void CChartXYAxis::SetXYAxisFontId( int _FontId )
	{
		if(_FontId >= 0){
			m_uXYAxisFontId = _FontId;
			Invalidate();
		}
	}

	//************************************
	// 函数名称: GetXYAxisFontId
	// 返回类型: int
	// 函数说明: 
	//************************************
	int CChartXYAxis::GetXYAxisFontId()
	{
		return m_uXYAxisFontId;
	}

	//************************************
	// 函数名称: SetXYAxisLineSize
	// 返回类型: void
	// 参数信息: UINT _LineSize
	// 函数说明: 
	//************************************
	void CChartXYAxis::SetXYAxisLineSize( UINT _LineSize )
	{
		m_uXYAxisLineSize	= _LineSize;
		Invalidate();
	}

	//************************************
	// 函数名称: GetXYAxisLineSize
	// 返回类型: UINT
	// 函数说明: 
	//************************************
	UINT CChartXYAxis::GetXYAxisLineSize()
	{
		return m_uXYAxisLineSize;
	}
	
	//************************************
	// 函数名称: SetLegendMaxWidth
	// 返回类型: void
	// 参数信息: UINT _LegendMaxWidth
	// 函数说明: 
	//************************************
	void CChartXYAxis::SetLegendMaxWidth( UINT _LegendMaxWidth )
	{
		m_uLegendMaxWidth	= _LegendMaxWidth;
		Invalidate();
	}

	//************************************
	// 函数名称: GetLegendMaxWidth
	// 返回类型: UINT
	// 函数说明: 
	//************************************
	UINT CChartXYAxis::GetLegendMaxWidth()
	{
		return m_uLegendMaxWidth;
	}

	//************************************
	// 函数名称: SetLegendMaxHeight
	// 返回类型: void
	// 参数信息: UINT _LegendMaxHeight
	// 函数说明: 
	//************************************
	void CChartXYAxis::SetLegendMaxHeight( UINT _LegendMaxHeight )
	{
		m_uLegendMaxHeight	= _LegendMaxHeight;
		Invalidate();
	}

	//************************************
	// 函数名称: GetLegendMaxHeight
	// 返回类型: UINT
	// 函数说明: 
	//************************************
	UINT CChartXYAxis::GetLegendMaxHeight()
	{
		return m_uLegendMaxHeight;
	}

	//************************************
	// 函数名称: GetMaxGraphWidth
	// 返回类型: int 
	// 函数说明: 
	//************************************
	int CChartXYAxis::GetMaxGraphWidth()
	{
		return m_iMaxGraphWidth;
	}

	//************************************
	// 函数名称: GetMaxGraphHeight
	// 返回类型: int 
	// 函数说明: 
	//************************************
	int CChartXYAxis::GetMaxGraphHeight()
	{
		return m_iMaxGraphHeight;
	}

	//************************************
	// 函数名称: SetMaxAxisLabelWidth
	// 返回类型: void
	// 参数信息: UINT _AxisLabelWidth
	// 函数说明: 
	//************************************
	void CChartXYAxis::SetMaxAxisLabelWidth( UINT _AxisLabelWidth )
	{
		m_uMaxAxisLabelWidth	= _AxisLabelWidth;
		Invalidate();
	}

	//************************************
	// 函数名称: GetMaxAxisLabelWidth
	// 返回类型: UINT
	// 函数说明: 
	//************************************
	UINT CChartXYAxis::GetMaxAxisLabelWidth()
	{
		return m_uMaxAxisLabelWidth;
	}

	//************************************
	// 函数名称: SetMaxAxisLabelHeight
	// 返回类型: void
	// 参数信息: UINT _AxisLabelHeight
	// 函数说明: 
	//************************************
	void CChartXYAxis::SetMaxAxisLabelHeight( UINT _AxisLabelHeight )
	{
		m_uMaxAxisLabelHeight = _AxisLabelHeight;
		Invalidate();
	}

	//************************************
	// 函数名称: GetMaxAxisLabelHeight
	// 返回类型: UINT
	// 函数说明: 
	//************************************
	UINT CChartXYAxis::GetMaxAxisLabelHeight()
	{
		return m_uMaxAxisLabelHeight;
	}

	//************************************
	// 函数名称: SetMaxDataLabelWidth
	// 返回类型: void
	// 参数信息: UINT _DataLabelWidth
	// 函数说明：#end$
	//************************************
	void CChartXYAxis::SetMaxDataLabelWidth( UINT _DataLabelWidth )
	{
		m_uMaxDataLabelWidth = _DataLabelWidth;
	}

	//************************************
	// 函数名称: GetMaxDataLabelWidth
	// 返回类型: UINT
	// 函数说明：#end$
	//************************************
	UINT CChartXYAxis::GetMaxDataLabelWidth()
	{
		return m_uMaxDataLabelWidth;
	}

	//************************************
	// 函数名称: SetLegendLocation
	// 返回类型: void
	// 参数信息: lalign _LegendLocation
	// 函数说明: 
	//************************************
	void CChartXYAxis::SetLegendLocation( lalign _LegendLocation )
	{
		m_uLegendLocation	= _LegendLocation;
		Invalidate();
	}

	//************************************
	// 函数名称: GetLegendLocation
	// 返回类型: UiLib::lalign
	// 函数说明: 
	//************************************
	UiLib::lalign CChartXYAxis::GetLegendLocation()
	{
		return m_uLegendLocation;
	}

	//************************************
	// 函数名称: SetXYAxisLineColor
	// 返回类型: void
	// 参数信息: DWORD _LineColor
	// 函数说明: 
	//************************************
	void CChartXYAxis::SetXYAxisLineColor( DWORD _LineColor )
	{
		m_dXYAxisLineColor = _LineColor;
		Invalidate();
	}

	//************************************
	// 函数名称: SetXYAxisLineColor
	// 返回类型: void
	// 参数信息: LPCTSTR _LineColor
	// 函数说明: 
	//************************************
	void CChartXYAxis::SetXYAxisLineColor( LPCTSTR _LineColor )
	{
		if(_LineColor){
			if( *_LineColor == _T('#'))
				_LineColor = ::CharNext(_LineColor);
			LPTSTR pstr = NULL;
			SetXYAxisLineColor(_tcstoul(_LineColor, &pstr, 16));
			Invalidate();
		}
	}

	//************************************
	// 函数名称: GetXYAxisLineColor
	// 返回类型: DWORD
	// 函数说明: 
	//************************************
	DWORD CChartXYAxis::GetXYAxisLineColor()
	{
		return m_dXYAxisLineColor;
	}

	//************************************
	// 函数名称: SetChartViewObj
	// 返回类型: void
	// 参数信息: CChartViewUI * _pChartViewUI
	// 函数说明: 
	//************************************
	void CChartXYAxis::SetChartViewObj( CChartViewUI* _pChartViewUI )
	{
		m_pChartView = _pChartViewUI;
	}

	//************************************
	// 函数名称: Invalidate
	// 返回类型: void
	// 函数说明: 
	//************************************
	void CChartXYAxis::Invalidate()
	{
		if(m_pChartView)
			m_pChartView->Invalidate();
	}

	void CChartXYAxis::DrawAxis(const RECT& rcPaint,RECT& rcItem )
	{
		rcItem.left		+= m_rcChartPadding.left;
		rcItem.top		+= m_rcChartPadding.top;
		rcItem.right	-= m_rcChartPadding.right;
		rcItem.bottom	-= m_rcChartPadding.bottom;

		RECT nRc		= rcItem;

		//计算图例面板占位
		if(m_uLegendLocation == LOCATION_TOP)
			nRc.top		= nRc.top + m_uLegendMaxHeight + 6;
		else if(m_uLegendLocation == LOCATION_BOTTOM)
			nRc.bottom	= nRc.bottom - m_uLegendMaxHeight - 6;
		else if(m_uLegendLocation == LOCATION_RIGHT)
			nRc.right	= nRc.right - m_uLegendMaxWidth;

		//计算刻度标签占位
		if(m_uAlignment == CHART_HORIZONTAL){
			//计算标签占位
			if(m_uLabelLocation == LOCATION_TOP)
				nRc.top += m_uMaxAxisLabelHeight;
			else if(m_uLabelLocation == LOCATION_BOTTOM)
				nRc.bottom -= m_uMaxAxisLabelHeight;

			//留出数据刻度占位
			nRc.left += m_uMaxDataLabelWidth;
		}
		else if(m_uAlignment == CHART_VERTICAL){

		}

		nRc.top += 5;

		m_iMaxGraphWidth	= nRc.right - nRc.left;
		m_iMaxGraphHeight	= nRc.bottom - nRc.top;

		if(m_uAlignment == CHART_HORIZONTAL && m_iMaxGraphWidth > 0 && m_iMaxGraphHeight > 0){
			m_dDataScale	= GetTickCount() == 0?nRc.bottom:1.00 * m_iMaxGraphHeight / ((GetMinTick() - GetMinTick()*2) + GetMaxTick());
			m_iAxissCale	= GetTickCount() == 0?nRc.bottom:nRc.top + (int)(GetMaxTick() * m_dDataScale);
			m_iMinBaseLine	= 0;

			//画Y轴
			RECT nRcYAxis = {0};
			nRcYAxis.left	= nRc.left;
			nRcYAxis.top	= nRc.top - 5;
			nRcYAxis.right	= nRc.left;
			nRcYAxis.bottom	= nRc.bottom + 3;

			m_pChartView->GetManager()->GetRenderCore()->DrawLine(nRcYAxis,m_uXYAxisLineSize,m_dXYAxisLineColor);

			//画Y轴刻度
			for(int nIndex = 0;nIndex < m_rDataAxis.GetSize();nIndex++){
				RECT nRcItemYAxis		= {0};
				RECT nRcItemYAxisEx		= {0};
				RECT nRcItemYText		= {0};

				int nValue = m_rDataAxis.GetAt(nIndex);


				int nValueScale			= (int)((nValue > 0?nValue:(nValue - nValue*2))* m_dDataScale);
				nRcItemYAxis.right		= nRcYAxis.left - 5;
				nRcItemYAxis.left		= nRcYAxis.left;
				nRcItemYAxis.top		= nValue > 0?m_iAxissCale - nValueScale:m_iAxissCale + nValueScale;
				nRcItemYAxis.bottom		= nRcItemYAxis.top;

				nRcItemYAxisEx.left		= nRcYAxis.left;
				nRcItemYAxisEx.right	= nRc.right;
				nRcItemYAxisEx.top		= nRcItemYAxis.top;
				nRcItemYAxisEx.bottom	= nRcItemYAxis.bottom;

				if(nValue == GetMaxTick())
					m_iMaxBaseLine		= nRcItemYAxisEx.top;
				if(nValue == GetMinTick())
					m_iMinBaseLine		= nRcItemYAxisEx.top;

				//绘制基线
				if(GetVisibleBaseLine())
					m_pChartView->GetManager()->GetRenderCore()->DrawLine(nRcItemYAxisEx,m_uXYAxisLineSize,RGB(CutColor(m_dXYAxisLineColor,20),CutColor(m_dXYAxisLineColor,20),CutColor(m_dXYAxisLineColor,20)));
		
				m_pChartView->GetManager()->GetRenderCore()->DrawLine(nRcItemYAxis,m_uXYAxisLineSize,m_dXYAxisLineColor);
				
				CDuiString sValue;
				sValue.Format(_T("%d%s"),nValue > 10000?nValue/1000:nValue,nValue > 10000?_T("K"):_T(""));

				nRcItemYText.left	= nRcYAxis.left - m_uMaxDataLabelWidth;
				nRcItemYText.right	= nRcYAxis.left - 5;
				nRcItemYText.top	= nRcItemYAxis.top - 5;
				nRcItemYText.bottom	= nRcItemYAxis.top + 5;
				CPaintManagerUI *pManager = m_pChartView->GetManager();
				pManager->GetRenderCore()->DrawText(pManager,nRcItemYText,sValue.GetData(),m_dXYAxisLineColor,m_uXYAxisFontId,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
			}

			//画X轴
			RECT nRcXAxis = {0};
			nRcXAxis.left	= nRcYAxis.left;
			nRcXAxis.top	= m_iAxissCale;
			nRcXAxis.right	= nRc.right;
			nRcXAxis.bottom	= nRcXAxis.top;
			m_pChartView->GetManager()->GetRenderCore()->DrawLine(nRcXAxis,m_uXYAxisLineSize,m_dXYAxisLineColor);

			double nGroupScale		= (double)(m_iMaxGraphWidth / m_rChartLabels.GetSize());
			double nItemScale		= (double)(nGroupScale / (m_pChartView->GetSeriesCount()+1));
			double nGroupSpace		= (double)(nItemScale / 2);
			double nItemSpace		= (double)(nGroupSpace / 10);

			//画X轴刻度
			for(int nSeriesIndex = 0;(UINT)nSeriesIndex < m_pChartView->GetSeriesCount();nSeriesIndex++){
				CChartSeries* pSeries = m_pChartView->GetSeries(nSeriesIndex);
				if(!pSeries)
					continue;
			
				for(int nIndex = 0;nIndex < m_rChartLabels.GetSize() && nIndex < pSeries->GetDataCount();nIndex++){
					CChartItem* pItem		= pSeries->GetSeriesData(nIndex);
					if(!pItem)
						continue;

					RECT nRcItemXBar		= {0};
					
					nRcItemXBar.left		= (int)(nRcYAxis.left + nGroupScale*nIndex + nGroupSpace + nItemScale*nSeriesIndex);
					nRcItemXBar.right		= (int)(nRcItemXBar.left + nItemScale - nItemSpace);
					nRcItemXBar.top			= pItem->GetDataValue()>0?(int)(m_iAxissCale - pItem->GetDataValue()*m_dDataScale):(int)(m_iAxissCale + (pItem->GetDataValue() - pItem->GetDataValue()*2)*m_dDataScale);
					nRcItemXBar.bottom		= m_iAxissCale;

					m_pChartView->GetManager()->GetRenderCore()->DrawGradient(nRcItemXBar,pSeries->GetSeriesColorA(),pSeries->GetSeriesColorB(),false);
					m_pChartView->GetManager()->GetRenderCore()->DrawRect(nRcItemXBar,1,m_dXYAxisLineColor);

					RECT nRcItemXAxis		= {0};

					nRcItemXAxis.left		= (int)(nRcYAxis.left + nGroupScale*(nIndex+1) - nItemSpace/2);
					nRcItemXAxis.right		= nRcItemXAxis.left;
					nRcItemXAxis.top		= m_iMinBaseLine + 2;
					nRcItemXAxis.bottom		= m_iMinBaseLine - 3;

					m_pChartView->GetManager()->GetRenderCore()->DrawLine(nRcItemXAxis,m_uXYAxisLineSize,RGB(CutColor(m_dXYAxisLineColor,75),CutColor(m_dXYAxisLineColor,75),CutColor(m_dXYAxisLineColor,75)));

					nRcItemXAxis.top		= m_iAxissCale + 2;
					nRcItemXAxis.bottom		= m_iAxissCale - 3;

					m_pChartView->GetManager()->GetRenderCore()->DrawLine(nRcItemXAxis,m_uXYAxisLineSize,m_dXYAxisLineColor);
	
					if(m_iAxissCale != m_iMinBaseLine){
						nRcItemXAxis.left		= nRcXAxis.right;
						nRcItemXAxis.right		= nRcXAxis.left +1;
						nRcItemXAxis.top		= m_iMinBaseLine;
						nRcItemXAxis.bottom		= m_iMinBaseLine;

						m_pChartView->GetManager()->GetRenderCore()->DrawLine(nRcItemXAxis,m_uXYAxisLineSize,RGB(CutColor(m_dXYAxisLineColor,75),CutColor(m_dXYAxisLineColor,75),CutColor(m_dXYAxisLineColor,75)));
					}

					CDuiString nRcGroupXText	= m_rChartLabels.GetAt(nIndex);
					RECT nRcItemXText		= {0};
					nRcItemXText.left		= (int)(nRcYAxis.left + nGroupScale*nIndex);
					nRcItemXText.right		= (int)(nRcYAxis.left + nGroupScale*(nIndex+1) - nItemSpace/2);
					nRcItemXText.top		= m_iMinBaseLine;
					nRcItemXText.bottom		= m_iMinBaseLine + m_uMaxAxisLabelHeight;
					CPaintManagerUI *pManager = m_pChartView->GetManager();
					pManager->GetRenderCore()->DrawText(pManager,nRcItemXText,nRcGroupXText.GetData(),m_dXYAxisLineColor,m_uXYAxisFontId,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
				}
			}
			//画数据图例
			if(m_pChartView->GetSeriesCount()){
				if(m_uLegendLocation == LOCATION_RIGHT){
					int	nSeriesLegendHeight	= m_uLegendMaxHeight*(m_pChartView->GetSeriesCount()+1)+8;
					int nSeriesLegendWidth	= m_uLegendMaxWidth - 32;

					if(m_iMaxGraphHeight > nSeriesLegendHeight){
						RECT nRcSeriesLegend	= {0};
						nRcSeriesLegend.left	= nRcXAxis.right + 8;
						nRcSeriesLegend.right	= nRcSeriesLegend.left + m_uLegendMaxWidth -8;
						nRcSeriesLegend.top		= nRc.top + (int)(m_iMaxGraphHeight - nSeriesLegendHeight)/2;
						nRcSeriesLegend.bottom	= nRc.bottom - (int)(m_iMaxGraphHeight - nSeriesLegendHeight)/2;

						m_pChartView->GetManager()->GetRenderCore()->DrawRect(nRcSeriesLegend,1,m_dXYAxisLineColor);

						for(int nSeriesIndex = 0;(UINT)nSeriesIndex < m_pChartView->GetSeriesCount();nSeriesIndex++){

							if(nSeriesIndex == 0){
								RECT nRcLegendTitleText		= {0};
								nRcLegendTitleText.left		= nRcSeriesLegend.left;
								nRcLegendTitleText.right	= nRcSeriesLegend.right;
								nRcLegendTitleText.top		= nRcSeriesLegend.top;
								nRcLegendTitleText.bottom	= nRcLegendTitleText.top+m_uLegendMaxHeight;
								CPaintManagerUI *pManager = m_pChartView->GetManager();
								pManager->GetRenderCore()->DrawText(pManager,nRcLegendTitleText,m_sLegendTitel.GetData(),m_dXYAxisLineColor,m_uXYAxisFontId,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
							}
							CChartSeries* pSeries	= m_pChartView->GetSeries(nSeriesIndex);

							if(!pSeries)
								continue;

							RECT nRcLegendText		= {0};
							nRcLegendText.left		= nRcSeriesLegend.left + 5;
							nRcLegendText.right		= nRcLegendText.left + m_uLegendMaxWidth - 40;
							nRcLegendText.top		= nRcSeriesLegend.top+m_uLegendMaxHeight + 2 + m_uLegendMaxHeight*nSeriesIndex;
							nRcLegendText.bottom	= nRcLegendText.top + m_uLegendMaxHeight + 2;
							
							CPaintManagerUI *pManager = m_pChartView->GetManager();
							pManager->GetRenderCore()->DrawText(pManager,nRcLegendText,pSeries->GetLegendText(),m_dXYAxisLineColor,m_uXYAxisFontId,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
							
							RECT nRcLegendBar		= {0};
							nRcLegendBar.left		= nRcLegendText.right + 5;
							nRcLegendBar.right		= nRcSeriesLegend.right - 5;
							nRcLegendBar.top		= nRcLegendText.top + 5;
							nRcLegendBar.bottom		= nRcLegendText.bottom - 5;

							m_pChartView->GetManager()->GetRenderCore()->DrawGradient(nRcLegendBar,pSeries->GetSeriesColorA(),pSeries->GetSeriesColorB(),false);
							m_pChartView->GetManager()->GetRenderCore()->DrawRect(nRcLegendBar,1,m_dXYAxisLineColor);
						}
					}
				}
				else if(m_uLegendLocation == LOCATION_TOP || m_uLegendLocation == LOCATION_BOTTOM){

					int nSeriesLegendHeight		= m_uLegendMaxHeight;
					int nSeriesLegendWidth		= m_uLegendMaxWidth*m_pChartView->GetSeriesCount();

					RECT nRcSeriesLegend		= {0};
					nRcSeriesLegend.left	= nRcYAxis.left + (int)(m_iMaxGraphWidth - nSeriesLegendWidth)/2 + 2;
					if(m_uLegendLocation == LOCATION_TOP){
						nRcSeriesLegend.top		= nRc.top - 6 - nSeriesLegendHeight - 5;
						nRcSeriesLegend.bottom	= nRc.top - 6 - 5;
					}
					else if(m_uLegendLocation == LOCATION_BOTTOM){
						nRcSeriesLegend.top		= m_uLabelLocation == LOCATION_BOTTOM?nRc.bottom + nSeriesLegendHeight:nRc.bottom + 6 + nSeriesLegendHeight;
						nRcSeriesLegend.bottom	= nRcSeriesLegend.top + m_uLegendMaxHeight;
					}
					nRcSeriesLegend.right		= nRcXAxis.right - (int)(m_iMaxGraphWidth - nSeriesLegendWidth)/2 - 2;

					m_pChartView->GetManager()->GetRenderCore()->DrawRect(nRcSeriesLegend,1,m_dXYAxisLineColor);

					for(int nSeriesIndex = 0;(UINT)nSeriesIndex < m_pChartView->GetSeriesCount();nSeriesIndex++){
						CChartSeries* pSeries	= m_pChartView->GetSeries(nSeriesIndex);

						if(!pSeries)
							continue;

						RECT nRcLegendText		= {0};
						nRcLegendText.left		= nRcSeriesLegend.left + m_uLegendMaxWidth * nSeriesIndex + 5;
						nRcLegendText.right		= nRcLegendText.left + m_uLegendMaxWidth - 48;
						nRcLegendText.top		= nRcSeriesLegend.top + 2;
						nRcLegendText.bottom	= nRcSeriesLegend.bottom;
						
						CPaintManagerUI *pManager = m_pChartView->GetManager();
						pManager->GetRenderCore()->DrawText(pManager,nRcLegendText,pSeries->GetLegendText(),m_dXYAxisLineColor,m_uXYAxisFontId,DT_LEFT|DT_VCENTER|DT_SINGLELINE);

						RECT nRcLegendBar		= {0};
						nRcLegendBar.left		= nRcLegendText.right + 5;
						nRcLegendBar.right		= nRcLegendText.left + m_uLegendMaxWidth - 15;
						nRcLegendBar.top		= nRcSeriesLegend.top + 4;
						nRcLegendBar.bottom		= nRcSeriesLegend.bottom - 4;

						m_pChartView->GetManager()->GetRenderCore()->DrawGradient(nRcLegendBar,pSeries->GetSeriesColorA(),pSeries->GetSeriesColorB(),false);
						m_pChartView->GetManager()->GetRenderCore()->DrawRect(nRcLegendBar,1,m_dXYAxisLineColor);
					}
				}
			}
		}
		else if(m_uAlignment == CHART_VERTICAL){

		}

		rcItem = nRc;
	}

	//************************************
	// 函数名称: CalLegendSize
	// 返回类型: void
	// 参数信息: CChartSeries * _ChartSeries
	// 函数说明: 
	//************************************
	void CChartXYAxis::CalLegendSize(HDC hDC)
	{
		for(int iIndex = 0;iIndex < m_rChartLabels.GetSize();iIndex++){
			CDuiString nText = m_rChartLabels.GetAt(iIndex).GetData();
		}
	}

	//************************************
	// 函数名称: CalMaxAxisLabelWidth
	// 返回类型: void
	// 参数信息: HDC hDC
	// 参数信息: CChartSeries * _ChartSeries
	// 函数说明: 
	//************************************
	void CChartXYAxis::CalMaxAxisLabelWidth( HDC hDC,CChartSeries* _ChartSeries )
	{
		for(int iIndex = 0;iIndex < m_rChartLabels.GetSize();iIndex++){
			CDuiString nText = m_rChartLabels.GetAt(iIndex).GetData();

			if(nText.GetLength()){
				SIZE nSize	= {0};
				GetTextExtentPoint32(hDC,nText.GetData(),nText.GetLength(),&nSize);

				if((UINT)nSize.cx > GetMaxAxisLabelWidth())
					SetMaxAxisLabelWidth(nSize.cx);
			}
		}
	}

	//************************************
	// 函数名称: CalMaxDataLabelWidth
	// 返回类型: void
	// 参数信息: HDC hDC
	// 参数信息: CChartSeries * _ChartSeries
	// 函数说明：#end$
	//************************************
	void CChartXYAxis::CalMaxDataLabelWidth( HDC hDC,CChartSeries* _ChartSeries )
	{
		for(int iIndex = 0;iIndex < m_rChartLabels.GetSize();iIndex++){
			CChartItem* pItem = _ChartSeries->GetSeriesData(iIndex);
			if(!pItem)
				continue;

			CDuiString nDataText;
			nDataText.Format(_T("%d%s"),pItem->GetDataValue() > 10000?(int)(pItem->GetDataValue()/1000):(int)(pItem->GetDataValue()),pItem->GetDataValue() > 10000?_T("K"):_T(""));

			if(nDataText.GetLength()){
				SIZE nSize = {0};
				GetTextExtentPoint32(hDC,nDataText.GetData(),nDataText.GetLength(),&nSize);
				if((UINT)nSize.cx > GetMaxDataLabelWidth())
					SetMaxDataLabelWidth(nSize.cx);
			}
		}
	}

	//************************************
	// 函数名称: CalAxisDataLimits
	// 返回类型: void
	// 参数信息: CChartSeries * _ChartSeries
	// 函数说明：#end$
	//************************************
	void CChartXYAxis::CalAxisDataLimits( CChartSeries* _ChartSeries )
	{
		for(int iIndex = 0;iIndex < m_rChartLabels.GetSize();iIndex++){
			CChartItem* pItem = _ChartSeries->GetSeriesData(iIndex);
			if(!pItem)
				continue;

			if(pItem->GetDataValue() > GetMaxTick())
				SetMaxTick((int)(pItem->GetDataValue()));
			if(pItem->GetDataValue() < GetMinTick())
				SetMinTick((int)(pItem->GetDataValue()));

			int aa = (GetMaxTick() - GetMinTick())/5;
			int bb =  GetSetpTick();

			if((UINT)((GetMaxTick() - GetMinTick())/5) > GetSetpTick())
				SetStepTick((int)((GetMaxTick() - GetMinTick())/5));

			if(pItem->GetDataValue() > GetMaxTick())
				SetMaxTick((int)(pItem->GetDataValue()));
			if(pItem->GetDataValue() < GetMinTick())
				SetMinTick((int)(pItem->GetDataValue()));
		}
	}

	//************************************
	// 函数名称: RemoveAllDataLabel
	// 返回类型: void
	// 函数说明: 
	//************************************
	void CChartXYAxis::RemoveAllDataLabel()
	{
		m_rChartLabels.Empty();
	}

	//************************************
	// 函数名称: GetGroupCount
	// 返回类型: UINT
	// 函数说明: 
	//************************************
	UINT CChartXYAxis::GetGroupCount()
	{
		return m_rChartLabels.GetSize();
	}

	//************************************
	// 函数名称: GetTickCount
	// 返回类型: UINT
	// 函数说明：#end$
	//************************************
	UINT CChartXYAxis::GetTickCount()
	{
		return m_rDataAxis.GetSize() + (m_bVisibelCustomTick ? m_rDataCustomAxis.GetSize():0);
	}

	//************************************
	// 函数名称: AddColor
	// 返回类型: DWORD
	// 参数信息: DWORD _SrcValue
	// 参数信息: DWORD _AddValue
	// 函数说明: 
	//************************************
	DWORD CChartXYAxis::AddColor( DWORD _SrcValue,DWORD _AddValue )
	{
		if((_SrcValue + _AddValue) > 255)
			return 255;

		return _SrcValue + _AddValue;
	}

	//************************************
	// 函数名称: CutColor
	// 返回类型: DWORD
	// 参数信息: DWORD _SrcValue
	// 参数信息: DWORD _CurValue
	// 函数说明: 
	//************************************
	DWORD CChartXYAxis::CutColor( DWORD _SrcValue,DWORD _CurValue )
	{
		if((_SrcValue - _CurValue) < 0)
			return 0;

		return _SrcValue - _CurValue;
	}

	//************************************
	// 函数名称: SetVisibleBaseLine
	// 返回类型: void
	// 参数信息: bool _Visible
	// 函数说明: 
	//************************************
	void CChartXYAxis::SetVisibleBaseLine( bool _Visible )
	{
		m_bVisibleBaseLine = _Visible;
	}

	//************************************
	// 函数名称: GetVisibleBaseLine
	// 返回类型: bool
	// 函数说明: 
	//************************************
	bool CChartXYAxis::GetVisibleBaseLine()
	{
		return m_bVisibleBaseLine;
	}

	/**************************************************************************************************************************************************************************************************/
	/*
	/**************************************************************************************************************************************************************************************************/

	//************************************
	// 函数名称: CChartViewUI
	// 返回类型: 
	// 参数信息: void
	// 函数说明: 
	//************************************
	CChartViewUI::CChartViewUI( void )
	{
		m_cXYAxis.SetChartViewObj(this);
	}

	//************************************
	// 函数名称: ~CChartViewUI
	// 返回类型: 
	// 函数说明: 
	//************************************
	CChartViewUI::~CChartViewUI(void)
	{
		RemoveAllData();
	}

	//************************************
	// 函数名称: GetClass
	// 返回类型: LPCTSTR
	// 函数说明: 
	//************************************
	LPCTSTR CChartViewUI::GetClass() const
	{
		return _T("ChartView");
	}

	//************************************
	// 函数名称: GetInterface
	// 返回类型: LPVOID
	// 参数信息: LPCTSTR pstrName
	// 函数说明: 
	//************************************
	LPVOID CChartViewUI::GetInterface( LPCTSTR pstrName )
	{
		if( _tcscmp(pstrName, _T("ChartView")) == 0 ) return static_cast<CChartViewUI*>(this);
		else return CHorizontalLayoutUI::GetInterface(pstrName);
	}
	
	//************************************
	// 函数名称: DoPaint
	// 返回类型: void
	// 参数信息: HDC hDC
	// 参数信息: const RECT & rcPaint
	// 函数说明: 
	//************************************
	void CChartViewUI::DoPaint( const RECT& rcPaint )
	{
		if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;
		CHorizontalLayoutUI::DoPaint(rcPaint);

		if(GetGroupCount()){
			RECT nItem = m_rcItem;
			m_cXYAxis.DrawAxis(rcPaint,nItem);
		}
	}

	//************************************
	// 函数名称: AddLabel
	// 返回类型: void
	// 参数信息: LPCTSTR _DataLabel
	// 函数说明: 
	//************************************
	void CChartViewUI::AddLabel( LPCTSTR _DataLabel )
	{
		m_cXYAxis.AddLabel(_DataLabel);
	}

	//************************************
	// 函数名称: AddSeries
	// 返回类型: void
	// 参数信息: CChartSeries * _ChartSeries
	// 函数说明: 
	//************************************
	void CChartViewUI::AddSeries( CChartSeries* _ChartSeries )
	{
		if(m_rChartSeries.GetSize() == 0)
			m_cXYAxis.ResetTick();

		if(_ChartSeries && m_rChartSeries.Add(_ChartSeries)){
			_ChartSeries->SetChartViewObj(this);
			CalLegendSize(_ChartSeries);
		}
		Invalidate();
	}

	CChartSeries* CChartViewUI::GetSeries( LPCTSTR _LegendText )
	{
		for(int iIndex = 0;iIndex < m_rChartSeries.GetSize();iIndex++){
			CChartSeries* pSeries = m_rChartSeries.GetAt(iIndex);
			if(pSeries && _tcscmp(pSeries->GetLegendText(),_LegendText) == 0)
				return pSeries;
		}
		return NULL;
	}

	CChartSeries* CChartViewUI::GetSeries( int nIndex )
	{
		return m_rChartSeries.GetAt(nIndex);
	}
	
	UiLib::CDuiString CChartViewUI::GetSeriesName( int nIndex )
	{
		CChartSeries* pSeries = m_rChartSeries.GetAt(nIndex);
		if(pSeries)
			return pSeries->GetLegendText();

		return _T("");
	}

	void CChartViewUI::SetVisibleSeries( LPCTSTR _LegendText,bool _Visible )
	{
		CChartSeries* pSeries = GetSeries(_LegendText);
		if(pSeries)
			pSeries->SetVisible(_Visible);
	}

	bool CChartViewUI::GetVisibleSeries( LPCTSTR _LegendText )
	{
		CChartSeries* pSeries = GetSeries(_LegendText);
		if(pSeries)
			return pSeries->GetVisible();

		return false;
	}

	void CChartViewUI::CalLegendSize(CChartSeries* _ChartSeries )
	{
		if(_ChartSeries && GetManager() && GetManager()->GetPaintDC()){
			SIZE nSize	= {0};

			CDuiString nSeriesLegendText = _ChartSeries->GetLegendText();

			GetTextExtentPoint32(GetManager()->GetPaintDC(),nSeriesLegendText.GetData(),nSeriesLegendText.GetLength(),&nSize);
			if((UINT)nSize.cx + 36 > m_cXYAxis.GetLegendMaxWidth())
				m_cXYAxis.SetLegendMaxWidth(nSize.cx+36);

			if((UINT)nSize.cy+6 > m_cXYAxis.GetLegendMaxHeight())
				m_cXYAxis.SetLegendMaxHeight(nSize.cy+6);

			m_cXYAxis.CalMaxAxisLabelWidth( GetManager()->GetPaintDC(),_ChartSeries);
			m_cXYAxis.CalMaxDataLabelWidth( GetManager()->GetPaintDC(),_ChartSeries);
			m_cXYAxis.CalAxisDataLimits(_ChartSeries);
		}
	}

	CChartXYAxis& CChartViewUI::GetXYAxis()
	{
		return m_cXYAxis;
	}
	
	void CChartViewUI::RemoveSeries( int _Index )
	{
		CChartSeries* pSeries = m_rChartSeries.GetAt(_Index);
		if(pSeries)
		{
			pSeries->RemoveSeriesDataAll();
			m_cXYAxis.ResetTick();
			m_cXYAxis.SetMaxDataLabelWidth(25);

			m_cXYAxis.CalMaxDataLabelWidth( GetManager()->GetPaintDC(),pSeries);
			m_cXYAxis.CalAxisDataLimits(pSeries);
			delete pSeries;
			pSeries = NULL;
			m_rChartSeries.Remove(_Index);
		}
	}

	void CChartViewUI::RemoveSeries( CChartSeries* _ChartSeries )
	{
		if(!_ChartSeries)
			return;

		for(int nIndex = 0;nIndex < m_rChartSeries.GetSize();nIndex++){
			CChartSeries* pSeries = m_rChartSeries.GetAt(nIndex);
			if(pSeries == _ChartSeries)
				RemoveSeries(nIndex);
		}
	}

	void CChartViewUI::RemoveAllData()
	{
		int nCount = m_rChartSeries.GetSize();
		for(int nIndex = 0;nIndex < nCount;nIndex++)
		{
			CChartSeries* pSeries = m_rChartSeries.GetAt(nIndex);
			if(pSeries)
			{
				pSeries->RemoveSeriesDataAll();
				m_cXYAxis.ResetTick();
				m_cXYAxis.SetMaxDataLabelWidth(25);

				m_cXYAxis.CalMaxDataLabelWidth( GetManager()->GetPaintDC(),pSeries);
				m_cXYAxis.CalAxisDataLimits(pSeries);
				delete pSeries;
				pSeries = NULL;
			}
		}
		for(int i = 0 ; i < nCount ; i++)
			m_rChartSeries.Remove(i);
		m_rChartSeries.Resize(0);
	}

	UINT CChartViewUI::GetGroupCount()
	{
		return m_cXYAxis.GetGroupCount();
	}

	UINT CChartViewUI::GetSeriesCount()
	{
		return m_rChartSeries.GetSize();
	}

}

