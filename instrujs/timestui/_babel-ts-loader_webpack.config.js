//Webpack requires this to work with directories
const path =  require('path')
const Extract = require('mini-css-extract-plugin')
const Compresseur = require('terser-webpack-plugin')
const HtmlInstaller = require('html-webpack-plugin')
const TsconfigPathsPlugin = require('tsconfig-paths-webpack-plugin')

// This is main configuration object that tells Webpackw what to do.
module.exports = {
    //path to entry paint
    entry: {
        main: path.resolve(__dirname, './src/index.ts')
    },
    //path and filename of the final output
    output: {
        path: path.resolve(__dirname, '../../data/instrujs/www/timestui'),
        filename: 'bundle.js'
    },
    resolve: {
        extensions: ['.js', '.ts', '.tsx'],
        alias: {
            idbclient: path.resolve(__dirname, './src/idbclient.ts'),
            InfluxDB$: path.resolve(__dirname, './influxdb-client/packages/core/src/InfluxDB.ts'),
            FluxTableMetaData$: path.resolve(__dirname, './influxdb-client/packages/core/src/query/FluxTableMetaData.ts'),
        }
    },
    module: {
        rules: [
            // Browserify / polyfill InfluxDB javascript client query part
            {
                test: /\.ts?$/,
                exclude: [
                    /(node_modules)/,
                    path.resolve(__dirname, './influxdb-client'),
                    path.resolve(__dirname, './src'),
                    path.resolve(__dirname, '../src')
                ],
                include: [
                    path.resolve(__dirname, './influxdb-client/packages/core/src/InfluxDB.ts'),
                    path.resolve(__dirname, './influxdb-client/packages/core/src/errors.ts'),
                    path.resolve(__dirname, './influxdb-client/packages/core/src/options.ts'),
                    path.resolve(__dirname, './influxdb-client/packages/core/src/QueryApi.ts'),
                    path.resolve(__dirname, './influxdb-client/packages/core/src/impl/ChunksToLines.ts'),
                    path.resolve(__dirname, './influxdb-client/packages/core/src/impl/completeCommunicationObserver.ts'),
                    path.resolve(__dirname, './influxdb-client/packages/core/src/impl/linesToTables.ts'),
                    path.resolve(__dirname, './influxdb-client/packages/core/src/impl/Logger.ts'),
                    path.resolve(__dirname, './influxdb-client/packages/core/src/impl/ObservableQuery.ts'),
                    path.resolve(__dirname, './influxdb-client/packages/core/src/impl/RetryBuffer.ts'),
                    path.resolve(__dirname, './influxdb-client/packages/core/src/impl/retryStrategy.ts'),
                    path.resolve(__dirname, './influxdb-client/packages/core/src/impl/QueryApiImpl.ts'),
                    path.resolve(__dirname, './influxdb-client/packages/core/src/impl/WriteApiImpl.ts'),
                    path.resolve(__dirname, './influxdb-client/packages/core/src/impl/browser/FetchTransport.ts'),
                    path.resolve(__dirname, './influxdb-client/packages/core/src/observable/symbol.ts'),
                    path.resolve(__dirname, './influxdb-client/packages/core/src/observable/types.ts'),
                    path.resolve(__dirname, './influxdb-client/packages/core/src/query/FluxTableColumn.ts'),
                    path.resolve(__dirname, './influxdb-client/packages/core/src/query/FluxTableMetaData.ts'),
                    path.resolve(__dirname, './influxdb-client/packages/core/src/util/currentTime.ts'),
                    path.resolve(__dirname, './influxdb-client/packages/core/src/util/escape.ts'),
                    path.resolve(__dirname, './influxdb-client/packages/core/src/util/LineSplitter.ts')
                    ],
                resolve: {
                    plugins: [
                        new TsconfigPathsPlugin({
                                configFile: 'tsconfig_influx.json'
                        })
                    ],
                },
                use: [
                    {
                        loader: 'babel-loader',
                        options: {
                            "presets": [
                              ["@babel/preset-env", {
                                  "useBuiltIns": "usage",
                                  "corejs": {"version": 3, "proposals": true},
                                  "debug": true,
                                  "targets": {
                                      "ie": "11",
                                      "safari": "6"
                                  }
                              }]
                          ],
                          plugins: ['@babel/plugin-transform-runtime'],
                          cacheDirectory: true
                      }
                    },
                    {
                        loader: 'ts-loader',
                            options: {
                                configFile: 'tsconfig_influx.json'
                            }
                    }
                ]
            },
           // The DashT code
           {
                test: /\.ts?$/,
                exclude: [
                    /(node_modules)/,
                    /(influxdb-client)/
                    // path.resolve(__dirname, './influxdb-client')
                ],
                include: [
                    path.resolve(__dirname, './src'),
                    path.resolve(__dirname, '../src')
                ],
                resolve: {
                    plugins: [
                        new TsconfigPathsPlugin({
                        })
                    ]
                },
                use: [
                    {
                        loader: 'babel-loader',
                        options: {
                            "presets": [
                              ["@babel/preset-env", {
                                  "useBuiltIns": "usage",
                                  "corejs": {"version": 3, "proposals": true},
                                  "debug": true,
                                  "targets": {
                                      "ie": "11",
                                      "safari": "6"
                                  }
                              }]
                          ],
                          plugins: ['@babel/plugin-transform-runtime'],
                          cacheDirectory: true
                      }
                    },
                    {
                        loader: 'ts-loader',
                            options: {
                                configFile: 'tsconfig.json'
                            }
                    }
                ]
            },
            {
                test: /\.js$/,
                exclude: [
                    /(node_modules)/,
                    /(influxdb-client)/
                    // path.resolve(__dirname, './influxdb-client')
                    ],
                include: [path.resolve(__dirname, './src'),
                          path.resolve(__dirname, '../src')
                         ],
                use: [
                    {
                        loader: 'babel-loader',
                        options: {
                            "presets": [
                              ["@babel/preset-env", {
                                  "useBuiltIns": "entry",
                                  "corejs": {"version": 3, "proposals": true},
                                  "debug": true,
                                  "targets": {
                                      "ie": "11",
                                      "safari": "6"
                                  }
                              }]
                            ],
                            plugins: ['@babel/plugin-transform-runtime'],
                            cacheDirectory: true
                        }
                    }
                ]
            },
            {
                test:/\.(sa|sc|c)ss$/,
                exclude: /(node_modules)/,
                include: [path.resolve(__dirname, './sass'),
                          path.resolve(__dirname, '../sass')],
                use: [
                    {
                        loader: Extract.loader
                    },
                    {
                        loader: 'css-loader'
                    },
                    {
                        loader: 'sass-loader',
                        options: {
                            implementation: require('sass')
                        }
                    }
                ]
            },
            {
                test: /\.(png|jpe?g|gif|svg)$/,
                exclude: /(node_modules)/,
                include: [path.resolve(__dirname, '../image')],
                use: [
                    {
                        loader: 'file-loader',
                        options: {
                            outputPath: 'images'
                        }
                    }
                ]
            },
            {
                test: /iface\.js$/,
                include: [path.resolve(__dirname, '../src')],
                exclude: /(node_modules)/,
                use: [
                    {
                        loader: 'exports-loader'
                    },
                ]
            },
        ],
    },
    optimization: {
        minimizer: [
            new Compresseur({
                cache: true,
                parallel: true,
                sourceMap: true
            }),
        ],
    },
    devtool: 'source-map',
    stats: {
        children: false,
        maxModules: 0
    },
    performance: {
        hints: false
    },
    plugins: [
        new Extract({
            filename: 'bundle.css'
        }),
        new HtmlInstaller({
            template: './html/index.html',
            filename: 'index.html'
        }),
    ],
    //default mode is production
    mode: 'development'
}
