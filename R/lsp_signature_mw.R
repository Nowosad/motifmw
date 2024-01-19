#' Spatial signatures in moving window
#'
#' Calculates the co-occurrence vector (cove) spatial signature in a moving window based on a provided categorical raster and a window matrix.
#'
#' @param x SpatRaster
#' @param window Moving window matrix.
#' @param neighbourhood The number of directions in which cell adjacencies are considered as neighbours: 4 (rook's case) or 8 (queen's case). The default is 4.
#' @param ordered The type of pairs considered. Either "ordered" (TRUE) or "unordered" (FALSE). The default is FALSE.
#' @param normalization Should the output vector be normalized? Either "none" or "pdf". The "pdf" option normalizes a vector to sum to one. The default is "pdf".
#' @param classes Which classes (categories) should be analyzed? The default is NULL, which means that the classes are calculated directly from the input data and all of them are used in the calculations.
#' @param ... Additional argument to [terra::focalCpp()]
#'
#' @return A SpatRaster with the spatial signatures of each cell in many layers.
#' @export
#'
#' @examples
#' library(terra)
#' r = rast(matrix(c(rep(1, 23), rep(NA, 3), rep(2, 23)), ncol = 7))
#' plot(r)
#' w = matrix(1, 3, 3)
#' result = lsp_signature_mw(x = r, window = w)
#' result
lsp_signature_mw = function(x, window, neighbourhood = 4, ordered = FALSE, normalization = "pdf", classes = NULL, ...){
  # prepare arguments
  directions = as.matrix(neighbourhood)
  if (is.null(classes)) classes = terra::unique(x)[[1]]
  # prepare window (-1 is a replacement for NA)
  window[is.na(window)] = -1
  if (any(classes %in% -1)) stop("It is not possible to have a class of `-1` in the input data", call. = FALSE)
  # run the code
  terra::focalCpp(x = x, w = window, fun = mw_cove_terra, raster_window = window,
                  directions = directions, classes = classes, ordered = ordered,
                  normalization = normalization, ...)
}
