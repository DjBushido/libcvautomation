/*
 * =====================================================================================
 *
 *       Filename:  libcvautomation-opencv.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/21/2012 08:34:21 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Bradlee Speice, bspeice@uncc.edu
 *   Organization:  MOSAIC at University of North Carolina at Charlotte
 *
 * =====================================================================================
 */

#include "libcvautomation-opencv.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  matchSubImage
 *      Returns:  CvPoint if a match is found, null if not
 * =====================================================================================
 */

CvPoint matchSubImage ( IplImage *rootImage, IplImage *subImage, int searchMethod, double threshold )
{
	/* We have the two OpenCV images we want, go ahead and find if there are any matches */
	IplImage	*result;
	CvPoint		minloc, maxloc; /* Location for the match - depending on search algorithm,
									the result may be in either minloc or maxloc */
	CvPoint		badpoint; /* (-1, -1), used to indicate an error */
	double		minval, maxval;
	int			rootImage_width, rootImage_height;
	int			subImage_width, subImage_height;
	int			result_width, result_height;

	badpoint.x = badpoint.y = -1;

	/* Make sure we have valid images */
	if ( rootImage == 0 || subImage == 0) {
		/* Otherwise return invalid */
		minloc.x = minloc.y = -1;
		return minloc;
	}

	/* Set up the parameters for our result image */
	rootImage_width		= rootImage->width;
	rootImage_height	= rootImage->height;
	subImage_width	= subImage->width;
	subImage_height	= subImage->height;
	result_width	= rootImage_width - subImage_width + 1;
	result_height	= rootImage_height - subImage_height + 1;

	/* Initialize our result image */
	result = cvCreateImage( cvSize( result_width, result_height ), IPL_DEPTH_32F, 1 );

	/* Compute the result image */
	cvMatchTemplate( rootImage, subImage, result, searchMethod );
	cvMinMaxLoc( result, &minval, &maxval, &minloc, &maxloc, 0 );

	/* Free memory for the result image
	 * Note that we don't control the root or sub image,
	 * so don't mess with those */
	cvReleaseImage( &result );

	/* Make sure that we have enough correlation to return a result,
	 * and then return the match location*/
	/* Return the match location */
	if ( searchMethod == CV_TM_SQDIFF || searchMethod == CV_TM_SQDIFF_NORMED )
	{
		if ( minval < threshold )
			return minloc;
		else
			return badpoint;
	}
	else
	{
		if ( maxval > threshold )
			return maxloc;
		else
			return badpoint;
	}

}		/* -----  end of function matchSubImage  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  matchSubImage_location
 *  Description:  Match a root image and sub image from filename
 * =====================================================================================
 */
CvPoint matchSubImage_location ( char *rootImage_location, char *subImage_location, int searchMethod, double threshold )
{
	/* This is basically a wrapper for matchSubImage( IplImage, IplImage )
	 * All we do is load the images from the given filenames, and then
	 * pass off the result to the above-named function */
	IplImage *rootImage;
	rootImage = cvLoadImage( rootImage_location, CV_LOAD_IMAGE_COLOR );

	IplImage *subImage;
	subImage = cvLoadImage( subImage_location, CV_LOAD_IMAGE_COLOR );

	CvPoint return_point;
	return_point.x = return_point.y = -1;

	/* Make sure we have good images */
	if ( rootImage == 0 || subImage == 0 )
	{
		/* Return error */
		return return_point;
	}

	return_point = matchSubImage( rootImage, subImage, searchMethod, threshold );

	/* Free up the memory we created */
	cvReleaseImage( &rootImage );
	cvReleaseImage( &subImage );

	/* Our return_point will already be NULL if there's no match */
	return return_point;

}		/* -----  end of function matchSubImage  ----- */