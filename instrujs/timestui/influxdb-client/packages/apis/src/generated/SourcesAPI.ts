import {InfluxDB} from '@influxdata/influxdb-client'
import {APIBase, RequestOptions} from '../APIBase'
import {Buckets, HealthCheck, Source, Sources} from './types'

export interface GetSourcesRequest {
  /** The organization name. */
  org?: string
}
export interface PostSourcesRequest {
  /** Source to create */
  body: Source
}
export interface GetSourcesIDRequest {
  /** The source ID. */
  sourceID: string
}
export interface PatchSourcesIDRequest {
  /** The source ID. */
  sourceID: string
  /** Source update */
  body: Source
}
export interface DeleteSourcesIDRequest {
  /** The source ID. */
  sourceID: string
}
export interface GetSourcesIDHealthRequest {
  /** The source ID. */
  sourceID: string
}
export interface GetSourcesIDBucketsRequest {
  /** The source ID. */
  sourceID: string
  /** The organization name. */
  org?: string
}
/**
 * Sources API
 */
export class SourcesAPI {
  // internal
  private base: APIBase

  /**
   * Creates SourcesAPI
   * @param influxDB - an instance that knows how to communicate with InfluxDB server
   */
  constructor(influxDB: InfluxDB) {
    this.base = new APIBase(influxDB)
  }
  /**
   * Get all sources.
   * See {@link https://v2.docs.influxdata.com/v2.0/api/#operation/GetSources }
   * @param request - request parameters and body (if supported)
   * @param requestOptions - optional transport options
   * @returns promise of response
   */
  getSources(
    request?: GetSourcesRequest,
    requestOptions?: RequestOptions
  ): Promise<Sources> {
    return this.base.request(
      'GET',
      `/api/v2/sources${this.base.queryString(request, ['org'])}`,
      request,
      requestOptions
    )
  }
  /**
   * Creates a source.
   * See {@link https://v2.docs.influxdata.com/v2.0/api/#operation/PostSources }
   * @param request - request parameters and body (if supported)
   * @param requestOptions - optional transport options
   * @returns promise of response
   */
  postSources(
    request: PostSourcesRequest,
    requestOptions?: RequestOptions
  ): Promise<Source> {
    return this.base.request(
      'POST',
      `/api/v2/sources`,
      request,
      requestOptions,
      'application/json'
    )
  }
  /**
   * Get a source.
   * See {@link https://v2.docs.influxdata.com/v2.0/api/#operation/GetSourcesID }
   * @param request - request parameters and body (if supported)
   * @param requestOptions - optional transport options
   * @returns promise of response
   */
  getSourcesID(
    request: GetSourcesIDRequest,
    requestOptions?: RequestOptions
  ): Promise<Source> {
    return this.base.request(
      'GET',
      `/api/v2/sources/${request.sourceID}`,
      request,
      requestOptions
    )
  }
  /**
   * Update a Source.
   * See {@link https://v2.docs.influxdata.com/v2.0/api/#operation/PatchSourcesID }
   * @param request - request parameters and body (if supported)
   * @param requestOptions - optional transport options
   * @returns promise of response
   */
  patchSourcesID(
    request: PatchSourcesIDRequest,
    requestOptions?: RequestOptions
  ): Promise<Source> {
    return this.base.request(
      'PATCH',
      `/api/v2/sources/${request.sourceID}`,
      request,
      requestOptions,
      'application/json'
    )
  }
  /**
   * Delete a source.
   * See {@link https://v2.docs.influxdata.com/v2.0/api/#operation/DeleteSourcesID }
   * @param request - request parameters and body (if supported)
   * @param requestOptions - optional transport options
   * @returns promise of response
   */
  deleteSourcesID(
    request: DeleteSourcesIDRequest,
    requestOptions?: RequestOptions
  ): Promise<void> {
    return this.base.request(
      'DELETE',
      `/api/v2/sources/${request.sourceID}`,
      request,
      requestOptions
    )
  }
  /**
   * Get the health of a source.
   * See {@link https://v2.docs.influxdata.com/v2.0/api/#operation/GetSourcesIDHealth }
   * @param request - request parameters and body (if supported)
   * @param requestOptions - optional transport options
   * @returns promise of response
   */
  getSourcesIDHealth(
    request: GetSourcesIDHealthRequest,
    requestOptions?: RequestOptions
  ): Promise<HealthCheck> {
    return this.base.request(
      'GET',
      `/api/v2/sources/${request.sourceID}/health`,
      request,
      requestOptions
    )
  }
  /**
   * Get buckets in a source.
   * See {@link https://v2.docs.influxdata.com/v2.0/api/#operation/GetSourcesIDBuckets }
   * @param request - request parameters and body (if supported)
   * @param requestOptions - optional transport options
   * @returns promise of response
   */
  getSourcesIDBuckets(
    request: GetSourcesIDBucketsRequest,
    requestOptions?: RequestOptions
  ): Promise<Buckets> {
    return this.base.request(
      'GET',
      `/api/v2/sources/${
        request.sourceID
      }/buckets${this.base.queryString(request, ['org'])}`,
      request,
      requestOptions
    )
  }
}
