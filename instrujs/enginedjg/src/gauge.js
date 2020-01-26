/* $Id: gauge.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

/*eslint camelcase: ['error', {'properties': 'never'}]*/
import JustGage from 'justgage'

export function createGauge(name, value, decimals, label, donut, symbol) {
    var hasDonut = donut || false
    var hasSymbol = symbol || ''
    return new JustGage({
        id: name,
        value: value,
        decimals: decimals,
        symbol: hasSymbol,
        label: label,
        min: 0,
        max: 100,
        donut: hasDonut,
        donutStartAngle: 270,
        pointer: true,
        pointerOptions: {
            toplength: -15,
            bottomlength: 10,
            bottomwidth: 12,
            color: '#8e8e93',
            stroke: '#ffffff',
            stroke_width: 3,
            stroke_linecap: 'round'
        },
        gaugeColor      : '#bcb9b9',
        labelFontColor  : '#262626',
        valueFontColor  : '#232b99',
        valueFontFamily : "Courier",
        relativeGaugeSize: true
    })
}
