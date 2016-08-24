/*
% Copyright (C) 2003-2009 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
% Copyright 1991-1999 E. I. du Pont de Nemours and Company
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            DDDD    CCCC  M   M                              %
%                            D   D  C      MM MM                              %
%                            D   D  C      M M M                              %
%                            D   D  C      M   M                              %
%                            DDDD    CCCC  M   M                              %
%                                                                             %
%                                                                             %
%                          Read DICOM Image Format.                           %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/colormap.h"
#include "magick/constitute.h"
#include "magick/enhance.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/tempfile.h"
#include "magick/utility.h"

/*
#define USE_GRAYMAP
#define GRAYSCALE_USES_PALETTE
*/

/* Defines for "best guess" fixes */
#define NEW_IMPLICIT_LOGIC
#define IGNORE_WINDOW_FOR_UNSPECIFIED_SCALE_TYPE

/*
  Function types for reading MSB/LSB shorts/longs
*/
typedef magick_uint16_t (DicomReadShortFunc)(Image *);
typedef magick_uint32_t (DicomReadLongFunc)(Image *);

/*
   DCM enums
*/
typedef enum
{
  DCM_TS_IMPL_LITTLE,
  DCM_TS_EXPL_LITTLE,
  DCM_TS_EXPL_BIG,
  DCM_TS_JPEG,
  DCM_TS_JPEG_LS,
  DCM_TS_JPEG_2000,
  DCM_TS_RLE
} Dicom_TS;

typedef enum
{
  DCM_MSB_LITTLE,
  DCM_MSB_BIG_PENDING,
  DCM_MSB_BIG
} Dicom_MSB;

typedef enum
{
  DCM_PI_MONOCHROME1,
  DCM_PI_MONOCHROME2,
  DCM_PI_PALETTE_COLOR,
  DCM_PI_RGB,
  DCM_PI_OTHER
} Dicom_PI;

typedef enum
{
  DCM_RT_OPTICAL_DENSITY,
  DCM_RT_HOUNSFIELD,
  DCM_RT_UNSPECIFIED,
  DCM_RT_UNKNOWN
} Dicom_RT;

typedef enum
{
  DCM_RS_NONE,
  DCM_RS_PRE,
  DCM_RS_POST
} Dicom_RS;

/*
  Dicom medical image declarations.
*/
typedef struct _DicomStream
{
  /*
    Values representing nature of image
  */
  unsigned long
    rows,
    columns;

  unsigned int
    number_scenes,
    samples_per_pixel,
    bits_allocated,
    significant_bits,
    bytes_per_pixel,
    max_value_in,
    max_value_out,
    high_bit,
    pixel_representation,
    interlace;

  Dicom_MSB
    msb_state;

  Dicom_PI
    phot_interp;  

  double
    window_center,
    window_width,
    rescale_intercept,
    rescale_slope;

  Dicom_TS
    transfer_syntax;

  Dicom_RT
    rescale_type;

  Dicom_RS
    rescaling;

  /*
    Array to store offset table for fragments within image
  */
  magick_uint32_t
    offset_ct;
  magick_uint32_t *
    offset_arr;

  /*
    Variables used to handle fragments and RLE compression
  */
  magick_uint32_t
    frag_bytes;

  magick_uint32_t
    rle_seg_ct,
    rle_seg_offsets[15];

  int
    rle_rep_ct,
    rle_rep_char;

  /*
    Max and minimum sample values within image used for post rescale mapping
  */
  int
    upper_lim,
    lower_lim;

  Quantum
    *rescale_map;

#if defined(USE_GRAYMAP)  
  unsigned short
    *graymap;
#endif

  /*
    Values representing last read element
  */
  unsigned short
    group,
    element;

  int
    index,
    datum;

  size_t
    quantum,
    length;

  unsigned char *
    data;

  /*
    Remaining fields for internal use by DCM_ReadElement and to read data
  */
  DicomReadShortFunc *
    funcReadShort;

  DicomReadLongFunc *
    funcReadLong;

  int
    explicit_file;

  unsigned int
    verbose;
} DicomStream;

/*
  Function type for parsing DICOM elements
*/
typedef MagickPassFail (DicomElemParseFunc)(Image *Image,DicomStream *dcm,ExceptionInfo *exception);

/*
  Forward declaration for parser functions
*/
static DicomElemParseFunc funcDCM_TransferSyntax;
static DicomElemParseFunc funcDCM_StudyDate;
static DicomElemParseFunc funcDCM_PatientName;
static DicomElemParseFunc funcDCM_TriggerTime;
static DicomElemParseFunc funcDCM_FieldOfView;
static DicomElemParseFunc funcDCM_SeriesNumber;
static DicomElemParseFunc funcDCM_ImagePosition;
static DicomElemParseFunc funcDCM_ImageOrientation;
static DicomElemParseFunc funcDCM_SliceLocation;
static DicomElemParseFunc funcDCM_SamplesPerPixel;
static DicomElemParseFunc funcDCM_PhotometricInterpretation;
static DicomElemParseFunc funcDCM_PlanarConfiguration;
static DicomElemParseFunc funcDCM_NumberOfFrames;
static DicomElemParseFunc funcDCM_Rows;
static DicomElemParseFunc funcDCM_Columns;
static DicomElemParseFunc funcDCM_BitsAllocated;
static DicomElemParseFunc funcDCM_BitsStored;
static DicomElemParseFunc funcDCM_HighBit;
static DicomElemParseFunc funcDCM_PixelRepresentation;
static DicomElemParseFunc funcDCM_WindowCenter;
static DicomElemParseFunc funcDCM_WindowWidth;
static DicomElemParseFunc funcDCM_RescaleIntercept;
static DicomElemParseFunc funcDCM_RescaleSlope;
static DicomElemParseFunc funcDCM_RescaleType;
static DicomElemParseFunc funcDCM_PaletteDescriptor;
static DicomElemParseFunc funcDCM_LUT;
static DicomElemParseFunc funcDCM_Palette;

/*
  Type for holding information on DICOM elements
*/
typedef struct _DicomInfo
{
  const unsigned short
    group,
    element;

  const char
    *vr,
    *description;

  DicomElemParseFunc
    * const pfunc;
} DicomInfo;

/*
  Array holding information on DICOM elements
*/
static const DicomInfo
  dicom_info[]=
  {
    { 0x0000, 0x0000, "UL", "Group Length", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0001, "UL", "Command Length to End", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0002, "UI", "Affected SOP Class UID", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0003, "UI", "Requested SOP Class UID", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0010, "LO", "Command Recognition Code", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0100, "US", "Command Field", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0110, "US", "Message ID", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0120, "US", "Message ID Being Responded To", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0200, "AE", "Initiator", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0300, "AE", "Receiver", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0400, "AE", "Find Location", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0600, "AE", "Move Destination", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0700, "US", "Priority", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0800, "US", "Data Set Type", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0850, "US", "Number of Matches", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0860, "US", "Response Sequence Number", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0900, "US", "Status", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0901, "AT", "Offending Element", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0902, "LO", "Exception Comment", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0903, "US", "Exception ID", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x1000, "UI", "Affected SOP Instance UID", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x1001, "UI", "Requested SOP Instance UID", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x1002, "US", "Event Type ID", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x1005, "AT", "Attribute Identifier List", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x1008, "US", "Action Type ID", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x1020, "US", "Number of Remaining Suboperations", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x1021, "US", "Number of Completed Suboperations", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x1022, "US", "Number of Failed Suboperations", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x1023, "US", "Number of Warning Suboperations", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x1030, "AE", "Move Originator Application Entity Title", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x1031, "US", "Move Originator Message ID", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x4000, "LO", "Dialog Receiver", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x4010, "LO", "Terminal Type", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x5010, "SH", "Message Set ID", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x5020, "SH", "End Message Set", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x5110, "LO", "Display Format", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x5120, "LO", "Page Position ID", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x5130, "LO", "Text Format ID", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x5140, "LO", "Normal Reverse", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x5150, "LO", "Add Gray Scale", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x5160, "LO", "Borders", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x5170, "IS", "Copies", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x5180, "LO", "OldMagnificationType", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x5190, "LO", "Erase", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x51a0, "LO", "Print", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x51b0, "US", "Overlays", (DicomElemParseFunc *) NULL },
    { 0x0002, 0x0000, "UL", "Meta Element Group Length", (DicomElemParseFunc *) NULL },
    { 0x0002, 0x0001, "OB", "File Meta Information Version", (DicomElemParseFunc *) NULL },
    { 0x0002, 0x0002, "UI", "Media Storage SOP Class UID", (DicomElemParseFunc *) NULL },
    { 0x0002, 0x0003, "UI", "Media Storage SOP Instance UID", (DicomElemParseFunc *) NULL },
    { 0x0002, 0x0010, "UI", "Transfer Syntax UID", (DicomElemParseFunc *) funcDCM_TransferSyntax },
    { 0x0002, 0x0012, "UI", "Implementation Class UID", (DicomElemParseFunc *) NULL },
    { 0x0002, 0x0013, "SH", "Implementation Version Name", (DicomElemParseFunc *) NULL },
    { 0x0002, 0x0016, "AE", "Source Application Entity Title", (DicomElemParseFunc *) NULL },
    { 0x0002, 0x0100, "UI", "Private Information Creator UID", (DicomElemParseFunc *) NULL },
    { 0x0002, 0x0102, "OB", "Private Information", (DicomElemParseFunc *) NULL },
    { 0x0003, 0x0000, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0003, 0x0008, "US", "ISI Command Field", (DicomElemParseFunc *) NULL },
    { 0x0003, 0x0011, "US", "Attach ID Application Code", (DicomElemParseFunc *) NULL },
    { 0x0003, 0x0012, "UL", "Attach ID Message Count", (DicomElemParseFunc *) NULL },
    { 0x0003, 0x0013, "DA", "Attach ID Date", (DicomElemParseFunc *) NULL },
    { 0x0003, 0x0014, "TM", "Attach ID Time", (DicomElemParseFunc *) NULL },
    { 0x0003, 0x0020, "US", "Message Type", (DicomElemParseFunc *) NULL },
    { 0x0003, 0x0030, "DA", "Max Waiting Date", (DicomElemParseFunc *) NULL },
    { 0x0003, 0x0031, "TM", "Max Waiting Time", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x0000, "UL", "File Set Group Length", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1130, "CS", "File Set ID", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1141, "CS", "File Set Descriptor File ID", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1142, "CS", "File Set Descriptor File Specific Character Set", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1200, "UL", "Root Directory Entity First Directory Record Offset", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1202, "UL", "Root Directory Entity Last Directory Record Offset", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1212, "US", "File Set Consistency Flag", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1220, "SQ", "Directory Record Sequence", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1400, "UL", "Next Directory Record Offset", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1410, "US", "Record In Use Flag", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1420, "UL", "Referenced Lower Level Directory Entity Offset", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1430, "CS", "Directory Record Type", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1432, "UI", "Private Record UID", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1500, "CS", "Referenced File ID", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1504, "UL", "MRDR Directory Record Offset", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1510, "UI", "Referenced SOP Class UID In File", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1511, "UI", "Referenced SOP Instance UID In File", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1512, "UI", "Referenced Transfer Syntax UID In File", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1600, "UL", "Number of References", (DicomElemParseFunc *) NULL },
    { 0x0005, 0x0000, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0006, 0x0000, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0000, "UL", "Identifying Group Length", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0001, "UL", "Length to End", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0005, "CS", "Specific Character Set", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0008, "CS", "Image Type", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0010, "LO", "Recognition Code", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0012, "DA", "Instance Creation Date", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0013, "TM", "Instance Creation Time", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0014, "UI", "Instance Creator UID", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0016, "UI", "SOP Class UID", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0018, "UI", "SOP Instance UID", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0020, "DA", "Study Date", (DicomElemParseFunc *) funcDCM_StudyDate },
    { 0x0008, 0x0021, "DA", "Series Date", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0022, "DA", "Acquisition Date", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0023, "DA", "Image Date", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0024, "DA", "Overlay Date", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0025, "DA", "Curve Date", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0030, "TM", "Study Time", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0031, "TM", "Series Time", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0032, "TM", "Acquisition Time", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0033, "TM", "Image Time", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0034, "TM", "Overlay Time", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0035, "TM", "Curve Time", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0040, "xs", "Old Data Set Type", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0041, "xs", "Old Data Set Subtype", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0042, "CS", "Nuclear Medicine Series Type", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0050, "SH", "Accession Number", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0052, "CS", "Query/Retrieve Level", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0054, "AE", "Retrieve AE Title", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0058, "UI", "Failed SOP Instance UID List", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0060, "CS", "Modality", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0062, "SQ", "Modality Subtype", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0064, "CS", "Conversion Type", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0068, "CS", "Presentation Intent Type", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0070, "LO", "Manufacturer", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0080, "LO", "Institution Name", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0081, "ST", "Institution Address", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0082, "SQ", "Institution Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0090, "PN", "Referring Physician's Name", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0092, "ST", "Referring Physician's Address", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0094, "SH", "Referring Physician's Telephone Numbers", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0100, "SH", "Code Value", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0102, "SH", "Coding Scheme Designator", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0103, "SH", "Coding Scheme Version", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0104, "LO", "Code Meaning", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0105, "CS", "Mapping Resource", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0106, "DT", "Context Group Version", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x010b, "CS", "Code Set Extension Flag", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x010c, "UI", "Private Coding Scheme Creator UID", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x010d, "UI", "Code Set Extension Creator UID", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x010f, "CS", "Context Identifier", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1000, "LT", "Network ID", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1010, "SH", "Station Name", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1030, "LO", "Study Description", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1032, "SQ", "Procedure Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x103e, "LO", "Series Description", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1040, "LO", "Institutional Department Name", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1048, "PN", "Physician of Record", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1050, "PN", "Performing Physician's Name", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1060, "PN", "Name of Physician(s) Reading Study", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1070, "PN", "Operator's Name", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1080, "LO", "Admitting Diagnosis Description", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1084, "SQ", "Admitting Diagnosis Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1090, "LO", "Manufacturer's Model Name", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1100, "SQ", "Referenced Results Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1110, "SQ", "Referenced Study Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1111, "SQ", "Referenced Study Component Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1115, "SQ", "Referenced Series Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1120, "SQ", "Referenced Patient Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1125, "SQ", "Referenced Visit Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1130, "SQ", "Referenced Overlay Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1140, "SQ", "Referenced Image Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1145, "SQ", "Referenced Curve Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1148, "SQ", "Referenced Previous Waveform", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x114a, "SQ", "Referenced Simultaneous Waveforms", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x114c, "SQ", "Referenced Subsequent Waveform", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1150, "UI", "Referenced SOP Class UID", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1155, "UI", "Referenced SOP Instance UID", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1160, "IS", "Referenced Frame Number", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1195, "UI", "Transaction UID", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1197, "US", "Failure Reason", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1198, "SQ", "Failed SOP Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1199, "SQ", "Referenced SOP Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2110, "CS", "Old Lossy Image Compression", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2111, "ST", "Derivation Description", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2112, "SQ", "Source Image Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2120, "SH", "Stage Name", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2122, "IS", "Stage Number", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2124, "IS", "Number of Stages", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2128, "IS", "View Number", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2129, "IS", "Number of Event Timers", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x212a, "IS", "Number of Views in Stage", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2130, "DS", "Event Elapsed Time(s)", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2132, "LO", "Event Timer Name(s)", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2142, "IS", "Start Trim", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2143, "IS", "Stop Trim", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2144, "IS", "Recommended Display Frame Rate", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2200, "CS", "Transducer Position", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2204, "CS", "Transducer Orientation", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2208, "CS", "Anatomic Structure", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2218, "SQ", "Anatomic Region Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2220, "SQ", "Anatomic Region Modifier Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2228, "SQ", "Primary Anatomic Structure Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2230, "SQ", "Primary Anatomic Structure Modifier Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2240, "SQ", "Transducer Position Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2242, "SQ", "Transducer Position Modifier Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2244, "SQ", "Transducer Orientation Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2246, "SQ", "Transducer Orientation Modifier Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2251, "SQ", "Anatomic Structure Space Or Region Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2253, "SQ", "Anatomic Portal Of Entrance Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2255, "SQ", "Anatomic Approach Direction Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2256, "ST", "Anatomic Perspective Description", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2257, "SQ", "Anatomic Perspective Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2258, "ST", "Anatomic Location Of Examining Instrument Description", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2259, "SQ", "Anatomic Location Of Examining Instrument Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x225a, "SQ", "Anatomic Structure Space Or Region Modifier Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x225c, "SQ", "OnAxis Background Anatomic Structure Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x4000, "LT", "Identifying Comments", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0000, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0001, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0002, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0003, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0004, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0005, "UN", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0006, "UN", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0007, "UN", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0008, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0009, "LT", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x000a, "IS", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x000b, "IS", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x000c, "IS", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x000d, "IS", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x000e, "IS", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x000f, "UN", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0010, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0011, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0012, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0013, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0014, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0015, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0016, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0017, "LT", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0018, "LT", "Data Set Identifier", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x001a, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x001e, "UI", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0020, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0021, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0022, "SH", "User Orientation", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0023, "SL", "Initiation Type", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0024, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0025, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0026, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0027, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0029, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x002a, "SL", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x002c, "LO", "Series Comments", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x002d, "SL", "Track Beat Average", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x002e, "FD", "Distance Prescribed", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x002f, "LT", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0030, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0031, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0032, "LT", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0034, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0035, "SL", "Gantry Locus Type", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0037, "SL", "Starting Heart Rate", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0038, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0039, "SL", "RR Window Offset", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x003a, "SL", "Percent Cycle Imaged", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x003e, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x003f, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0040, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0041, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0042, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0043, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0050, "LT", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0051, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0060, "LT", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0061, "LT", "Series Unique Identifier", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0070, "LT", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0080, "LT", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0091, "LT", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x00e2, "LT", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x00e3, "UI", "Equipment UID", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x00e6, "SH", "Genesis Version Now", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x00e7, "UL", "Exam Record Checksum", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x00e8, "UL", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x00e9, "SL", "Actual Series Data Time Stamp", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x00f2, "UN", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x00f3, "UN", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x00f4, "LT", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x00f5, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x00f6, "LT", "PDM Data Object Type Extension", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x00f8, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x00fb, "IS", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x1002, "OB", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x1003, "OB", "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x1010, "UN", "?", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x0000, "UL", "Patient Group Length", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x0010, "PN", "Patient's Name", (DicomElemParseFunc *) funcDCM_PatientName },
    { 0x0010, 0x0020, "LO", "Patient's ID", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x0021, "LO", "Issuer of Patient's ID", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x0030, "DA", "Patient's Birth Date", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x0032, "TM", "Patient's Birth Time", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x0040, "CS", "Patient's Sex", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x0050, "SQ", "Patient's Insurance Plan Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x1000, "LO", "Other Patient's ID's", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x1001, "PN", "Other Patient's Names", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x1005, "PN", "Patient's Birth Name", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x1010, "AS", "Patient's Age", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x1020, "DS", "Patient's Size", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x1030, "DS", "Patient's Weight", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x1040, "LO", "Patient's Address", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x1050, "LT", "Insurance Plan Identification", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x1060, "PN", "Patient's Mother's Birth Name", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x1080, "LO", "Military Rank", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x1081, "LO", "Branch of Service", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x1090, "LO", "Medical Record Locator", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x2000, "LO", "Medical Alerts", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x2110, "LO", "Contrast Allergies", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x2150, "LO", "Country of Residence", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x2152, "LO", "Region of Residence", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x2154, "SH", "Patients Telephone Numbers", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x2160, "SH", "Ethnic Group", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x2180, "SH", "Occupation", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x21a0, "CS", "Smoking Status", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x21b0, "LT", "Additional Patient History", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x21c0, "US", "Pregnancy Status", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x21d0, "DA", "Last Menstrual Date", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x21f0, "LO", "Patients Religious Preference", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x4000, "LT", "Patient Comments", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0001, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0002, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0003, "LT", "Patient UID", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0004, "LT", "Patient ID", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x000a, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x000b, "SL", "Effective Series Duration", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x000c, "SL", "Num Beats", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x000d, "LO", "Radio Nuclide Name", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0010, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0011, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0012, "LO", "Dataset Name", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0013, "LO", "Dataset Type", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0015, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0016, "SL", "Energy Number", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0017, "SL", "RR Interval Window Number", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0018, "SL", "MG Bin Number", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0019, "FD", "Radius Of Rotation", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x001a, "SL", "Detector Count Zone", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x001b, "SL", "Num Energy Windows", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x001c, "SL", "Energy Offset", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x001d, "SL", "Energy Range", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x001f, "SL", "Image Orientation", (DicomElemParseFunc *) funcDCM_ImageOrientation },
    { 0x0011, 0x0020, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0021, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0022, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0023, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0024, "SL", "FOV Mask Y Cutoff Angle", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0025, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0026, "SL", "Table Orientation", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0027, "SL", "ROI Top Left", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0028, "SL", "ROI Bottom Right", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0030, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0031, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0032, "UN", "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0033, "LO", "Energy Correct Name", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0034, "LO", "Spatial Correct Name", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0035, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0036, "LO", "Uniformity Correct Name", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0037, "LO", "Acquisition Specific Correct Name", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0038, "SL", "Byte Order", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x003a, "SL", "Picture Format", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x003b, "FD", "Pixel Scale", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x003c, "FD", "Pixel Offset", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x003e, "SL", "FOV Shape", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x003f, "SL", "Dataset Flags", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0040, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0041, "LT", "Medical Alerts", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0042, "LT", "Contrast Allergies", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0044, "FD", "Threshold Center", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0045, "FD", "Threshold Width", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0046, "SL", "Interpolation Type", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0055, "FD", "Period", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0056, "FD", "ElapsedTime", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x00a1, "DA", "Patient Registration Date", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x00a2, "TM", "Patient Registration Time", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x00b0, "LT", "Patient Last Name", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x00b2, "LT", "Patient First Name", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x00b4, "LT", "Patient Hospital Status", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x00bc, "TM", "Current Location Time", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x00c0, "LT", "Patient Insurance Status", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x00d0, "LT", "Patient Billing Type", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x00d2, "LT", "Patient Billing Address", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0000, "LT", "Modifying Physician", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0010, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0011, "SL", "?", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0012, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0016, "SL", "AutoTrack Peak", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0017, "SL", "AutoTrack Width", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0018, "FD", "Transmission Scan Time", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0019, "FD", "Transmission Mask Width", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x001a, "FD", "Copper Attenuator Thickness", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x001c, "FD", "?", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x001d, "FD", "?", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x001e, "FD", "Tomo View Offset", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0020, "LT", "Patient Name", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0022, "LT", "Patient Id", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0026, "LT", "Study Comments", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0030, "DA", "Patient Birthdate", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0031, "DS", "Patient Weight", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0032, "LT", "Patients Maiden Name", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0033, "LT", "Referring Physician", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0034, "LT", "Admitting Diagnosis", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0035, "LT", "Patient Sex", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0040, "LT", "Procedure Description", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0042, "LT", "Patient Rest Direction", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0044, "LT", "Patient Position", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0046, "LT", "View Direction", (DicomElemParseFunc *) NULL },
    { 0x0015, 0x0001, "DS", "Stenosis Calibration Ratio", (DicomElemParseFunc *) NULL },
    { 0x0015, 0x0002, "DS", "Stenosis Magnification", (DicomElemParseFunc *) NULL },
    { 0x0015, 0x0003, "DS", "Cardiac Calibration Ratio", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0000, "UL", "Acquisition Group Length", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0010, "LO", "Contrast/Bolus Agent", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0012, "SQ", "Contrast/Bolus Agent Sequence", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0014, "SQ", "Contrast/Bolus Administration Route Sequence", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0015, "CS", "Body Part Examined", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0020, "CS", "Scanning Sequence", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0021, "CS", "Sequence Variant", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0022, "CS", "Scan Options", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0023, "CS", "MR Acquisition Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0024, "SH", "Sequence Name", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0025, "CS", "Angio Flag", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0026, "SQ", "Intervention Drug Information Sequence", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0027, "TM", "Intervention Drug Stop Time", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0028, "DS", "Intervention Drug Dose", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0029, "SQ", "Intervention Drug Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x002a, "SQ", "Additional Drug Sequence", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0030, "LO", "Radionuclide", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0031, "LO", "Radiopharmaceutical", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0032, "DS", "Energy Window Centerline", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0033, "DS", "Energy Window Total Width", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0034, "LO", "Intervention Drug Name", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0035, "TM", "Intervention Drug Start Time", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0036, "SQ", "Intervention Therapy Sequence", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0037, "CS", "Therapy Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0038, "CS", "Intervention Status", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0039, "CS", "Therapy Description", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0040, "IS", "Cine Rate", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0050, "DS", "Slice Thickness", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0060, "DS", "KVP", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0070, "IS", "Counts Accumulated", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0071, "CS", "Acquisition Termination Condition", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0072, "DS", "Effective Series Duration", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0073, "CS", "Acquisition Start Condition", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0074, "IS", "Acquisition Start Condition Data", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0075, "IS", "Acquisition Termination Condition Data", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0080, "DS", "Repetition Time", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0081, "DS", "Echo Time", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0082, "DS", "Inversion Time", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0083, "DS", "Number of Averages", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0084, "DS", "Imaging Frequency", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0085, "SH", "Imaged Nucleus", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0086, "IS", "Echo Number(s)", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0087, "DS", "Magnetic Field Strength", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0088, "DS", "Spacing Between Slices", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0089, "IS", "Number of Phase Encoding Steps", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0090, "DS", "Data Collection Diameter", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0091, "IS", "Echo Train Length", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0093, "DS", "Percent Sampling", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0094, "DS", "Percent Phase Field of View", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0095, "DS", "Pixel Bandwidth", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1000, "LO", "Device Serial Number", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1004, "LO", "Plate ID", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1010, "LO", "Secondary Capture Device ID", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1012, "DA", "Date of Secondary Capture", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1014, "TM", "Time of Secondary Capture", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1016, "LO", "Secondary Capture Device Manufacturer", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1018, "LO", "Secondary Capture Device Manufacturer Model Name", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1019, "LO", "Secondary Capture Device Software Version(s)", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1020, "LO", "Software Version(s)", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1022, "SH", "Video Image Format Acquired", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1023, "LO", "Digital Image Format Acquired", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1030, "LO", "Protocol Name", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1040, "LO", "Contrast/Bolus Route", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1041, "DS", "Contrast/Bolus Volume", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1042, "TM", "Contrast/Bolus Start Time", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1043, "TM", "Contrast/Bolus Stop Time", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1044, "DS", "Contrast/Bolus Total Dose", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1045, "IS", "Syringe Counts", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1046, "DS", "Contrast Flow Rate", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1047, "DS", "Contrast Flow Duration", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1048, "CS", "Contrast/Bolus Ingredient", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1049, "DS", "Contrast/Bolus Ingredient Concentration", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1050, "DS", "Spatial Resolution", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1060, "DS", "Trigger Time", (DicomElemParseFunc *) funcDCM_TriggerTime },
    { 0x0018, 0x1061, "LO", "Trigger Source or Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1062, "IS", "Nominal Interval", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1063, "DS", "Frame Time", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1064, "LO", "Framing Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1065, "DS", "Frame Time Vector", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1066, "DS", "Frame Delay", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1067, "DS", "Image Trigger Delay", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1068, "DS", "Group Time Offset", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1069, "DS", "Trigger Time Offset", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x106a, "CS", "Synchronization Trigger", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x106b, "UI", "Synchronization Frame of Reference", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x106e, "UL", "Trigger Sample Position", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1070, "LO", "Radiopharmaceutical Route", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1071, "DS", "Radiopharmaceutical Volume", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1072, "TM", "Radiopharmaceutical Start Time", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1073, "TM", "Radiopharmaceutical Stop Time", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1074, "DS", "Radionuclide Total Dose", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1075, "DS", "Radionuclide Half Life", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1076, "DS", "Radionuclide Positron Fraction", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1077, "DS", "Radiopharmaceutical Specific Activity", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1080, "CS", "Beat Rejection Flag", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1081, "IS", "Low R-R Value", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1082, "IS", "High R-R Value", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1083, "IS", "Intervals Acquired", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1084, "IS", "Intervals Rejected", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1085, "LO", "PVC Rejection", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1086, "IS", "Skip Beats", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1088, "IS", "Heart Rate", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1090, "IS", "Cardiac Number of Images", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1094, "IS", "Trigger Window", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1100, "DS", "Reconstruction Diameter", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1110, "DS", "Distance Source to Detector", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1111, "DS", "Distance Source to Patient", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1114, "DS", "Estimated Radiographic Magnification Factor", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1120, "DS", "Gantry/Detector Tilt", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1121, "DS", "Gantry/Detector Slew", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1130, "DS", "Table Height", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1131, "DS", "Table Traverse", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1134, "CS", "Table Motion", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1135, "DS", "Table Vertical Increment", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1136, "DS", "Table Lateral Increment", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1137, "DS", "Table Longitudinal Increment", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1138, "DS", "Table Angle", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x113a, "CS", "Table Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1140, "CS", "Rotation Direction", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1141, "DS", "Angular Position", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1142, "DS", "Radial Position", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1143, "DS", "Scan Arc", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1144, "DS", "Angular Step", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1145, "DS", "Center of Rotation Offset", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1146, "DS", "Rotation Offset", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1147, "CS", "Field of View Shape", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1149, "IS", "Field of View Dimension(s)", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1150, "IS", "Exposure Time", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1151, "IS", "X-ray Tube Current", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1152, "IS", "Exposure", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1153, "IS", "Exposure in uAs", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1154, "DS", "AveragePulseWidth", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1155, "CS", "RadiationSetting", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1156, "CS", "Rectification Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x115a, "CS", "RadiationMode", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x115e, "DS", "ImageAreaDoseProduct", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1160, "SH", "Filter Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1161, "LO", "TypeOfFilters", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1162, "DS", "IntensifierSize", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1164, "DS", "ImagerPixelSpacing", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1166, "CS", "Grid", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1170, "IS", "Generator Power", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1180, "SH", "Collimator/Grid Name", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1181, "CS", "Collimator Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1182, "IS", "Focal Distance", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1183, "DS", "X Focus Center", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1184, "DS", "Y Focus Center", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1190, "DS", "Focal Spot(s)", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1191, "CS", "Anode Target Material", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x11a0, "DS", "Body Part Thickness", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x11a2, "DS", "Compression Force", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1200, "DA", "Date of Last Calibration", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1201, "TM", "Time of Last Calibration", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1210, "SH", "Convolution Kernel", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1240, "IS", "Upper/Lower Pixel Values", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1242, "IS", "Actual Frame Duration", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1243, "IS", "Count Rate", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1244, "US", "Preferred Playback Sequencing", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1250, "SH", "Receiving Coil", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1251, "SH", "Transmitting Coil", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1260, "SH", "Plate Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1261, "LO", "Phosphor Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1300, "DS", "Scan Velocity", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1301, "CS", "Whole Body Technique", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1302, "IS", "Scan Length", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1310, "US", "Acquisition Matrix", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1312, "CS", "Phase Encoding Direction", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1314, "DS", "Flip Angle", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1315, "CS", "Variable Flip Angle Flag", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1316, "DS", "SAR", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1318, "DS", "dB/dt", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1400, "LO", "Acquisition Device Processing Description", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1401, "LO", "Acquisition Device Processing Code", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1402, "CS", "Cassette Orientation", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1403, "CS", "Cassette Size", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1404, "US", "Exposures on Plate", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1405, "IS", "Relative X-ray Exposure", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1450, "DS", "Column Angulation", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1460, "DS", "Tomo Layer Height", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1470, "DS", "Tomo Angle", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1480, "DS", "Tomo Time", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1490, "CS", "Tomo Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1491, "CS", "Tomo Class", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1495, "IS", "Number of Tomosynthesis Source Images", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1500, "CS", "PositionerMotion", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1508, "CS", "Positioner Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1510, "DS", "PositionerPrimaryAngle", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1511, "DS", "PositionerSecondaryAngle", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1520, "DS", "PositionerPrimaryAngleIncrement", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1521, "DS", "PositionerSecondaryAngleIncrement", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1530, "DS", "DetectorPrimaryAngle", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1531, "DS", "DetectorSecondaryAngle", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1600, "CS", "Shutter Shape", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1602, "IS", "Shutter Left Vertical Edge", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1604, "IS", "Shutter Right Vertical Edge", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1606, "IS", "Shutter Upper Horizontal Edge", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1608, "IS", "Shutter Lower Horizonta lEdge", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1610, "IS", "Center of Circular Shutter", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1612, "IS", "Radius of Circular Shutter", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1620, "IS", "Vertices of Polygonal Shutter", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1622, "US", "Shutter Presentation Value", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1623, "US", "Shutter Overlay Group", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1700, "CS", "Collimator Shape", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1702, "IS", "Collimator Left Vertical Edge", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1704, "IS", "Collimator Right Vertical Edge", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1706, "IS", "Collimator Upper Horizontal Edge", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1708, "IS", "Collimator Lower Horizontal Edge", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1710, "IS", "Center of Circular Collimator", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1712, "IS", "Radius of Circular Collimator", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1720, "IS", "Vertices of Polygonal Collimator", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1800, "CS", "Acquisition Time Synchronized", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1801, "SH", "Time Source", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1802, "CS", "Time Distribution Protocol", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x4000, "LT", "Acquisition Comments", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5000, "SH", "Output Power", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5010, "LO", "Transducer Data", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5012, "DS", "Focus Depth", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5020, "LO", "Processing Function", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5021, "LO", "Postprocessing Function", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5022, "DS", "Mechanical Index", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5024, "DS", "Thermal Index", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5026, "DS", "Cranial Thermal Index", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5027, "DS", "Soft Tissue Thermal Index", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5028, "DS", "Soft Tissue-Focus Thermal Index", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5029, "DS", "Soft Tissue-Surface Thermal Index", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5030, "DS", "Dynamic Range", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5040, "DS", "Total Gain", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5050, "IS", "Depth of Scan Field", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5100, "CS", "Patient Position", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5101, "CS", "View Position", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5104, "SQ", "Projection Eponymous Name Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5210, "DS", "Image Transformation Matrix", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5212, "DS", "Image Translation Vector", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6000, "DS", "Sensitivity", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6011, "IS", "Sequence of Ultrasound Regions", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6012, "US", "Region Spatial Format", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6014, "US", "Region Data Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6016, "UL", "Region Flags", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6018, "UL", "Region Location Min X0", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x601a, "UL", "Region Location Min Y0", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x601c, "UL", "Region Location Max X1", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x601e, "UL", "Region Location Max Y1", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6020, "SL", "Reference Pixel X0", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6022, "SL", "Reference Pixel Y0", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6024, "US", "Physical Units X Direction", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6026, "US", "Physical Units Y Direction", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6028, "FD", "Reference Pixel Physical Value X", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x602a, "US", "Reference Pixel Physical Value Y", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x602c, "US", "Physical Delta X", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x602e, "US", "Physical Delta Y", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6030, "UL", "Transducer Frequency", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6031, "CS", "Transducer Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6032, "UL", "Pulse Repetition Frequency", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6034, "FD", "Doppler Correction Angle", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6036, "FD", "Steering Angle", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6038, "UL", "Doppler Sample Volume X Position", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x603a, "UL", "Doppler Sample Volume Y Position", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x603c, "UL", "TM-Line Position X0", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x603e, "UL", "TM-Line Position Y0", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6040, "UL", "TM-Line Position X1", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6042, "UL", "TM-Line Position Y1", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6044, "US", "Pixel Component Organization", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6046, "UL", "Pixel Component Mask", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6048, "UL", "Pixel Component Range Start", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x604a, "UL", "Pixel Component Range Stop", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x604c, "US", "Pixel Component Physical Units", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x604e, "US", "Pixel Component Data Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6050, "UL", "Number of Table Break Points", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6052, "UL", "Table of X Break Points", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6054, "FD", "Table of Y Break Points", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6056, "UL", "Number of Table Entries", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6058, "UL", "Table of Pixel Values", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x605a, "FL", "Table of Parameter Values", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7000, "CS", "Detector Conditions Nominal Flag", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7001, "DS", "Detector Temperature", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7004, "CS", "Detector Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7005, "CS", "Detector Configuration", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7006, "LT", "Detector Description", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7008, "LT", "Detector Mode", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x700a, "SH", "Detector ID", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x700c, "DA", "Date of Last Detector Calibration ", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x700e, "TM", "Time of Last Detector Calibration", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7010, "IS", "Exposures on Detector Since Last Calibration", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7011, "IS", "Exposures on Detector Since Manufactured", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7012, "DS", "Detector Time Since Last Exposure", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7014, "DS", "Detector Active Time", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7016, "DS", "Detector Activation Offset From Exposure", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x701a, "DS", "Detector Binning", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7020, "DS", "Detector Element Physical Size", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7022, "DS", "Detector Element Spacing", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7024, "CS", "Detector Active Shape", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7026, "DS", "Detector Active Dimensions", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7028, "DS", "Detector Active Origin", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7030, "DS", "Field of View Origin", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7032, "DS", "Field of View Rotation", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7034, "CS", "Field of View Horizontal Flip", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7040, "LT", "Grid Absorbing Material", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7041, "LT", "Grid Spacing Material", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7042, "DS", "Grid Thickness", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7044, "DS", "Grid Pitch", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7046, "IS", "Grid Aspect Ratio", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7048, "DS", "Grid Period", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x704c, "DS", "Grid Focal Distance", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7050, "LT", "Filter Material", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7052, "DS", "Filter Thickness Minimum", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7054, "DS", "Filter Thickness Maximum", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7060, "CS", "Exposure Control Mode", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7062, "LT", "Exposure Control Mode Description", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7064, "CS", "Exposure Status", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7065, "DS", "Phototimer Setting", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0000, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0001, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0002, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0003, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0004, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0005, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0006, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0007, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0008, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0009, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x000a, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x000b, "DS", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x000c, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x000d, "TM", "Time", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x000e, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x000f, "DS", "Horizontal Frame Of Reference", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0010, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0011, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0012, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0013, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0014, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0015, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0016, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0017, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0018, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0019, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x001a, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x001b, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x001c, "CS", "Dose", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x001d, "IS", "Side Mark", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x001e, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x001f, "DS", "Exposure Duration", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0020, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0021, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0022, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0023, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0024, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0025, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0026, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0027, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0028, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0029, "IS", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x002a, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x002b, "DS", "Xray Off Position", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x002c, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x002d, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x002e, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x002f, "DS", "Trigger Frequency", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0030, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0031, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0032, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0033, "UN", "ECG 2 Offset 2", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0034, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0036, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0038, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0039, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x003a, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x003b, "LT", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x003c, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x003e, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x003f, "UN", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0040, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0041, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0042, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0043, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0044, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0045, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0046, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0047, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0048, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0049, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x004a, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x004b, "SL", "Data Size For Scan Data", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x004c, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x004e, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0050, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0051, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0052, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0053, "LT", "Barcode", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0054, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0055, "DS", "Receiver Reference Gain", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0056, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0057, "SS", "CT Water Number", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0058, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x005a, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x005c, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x005d, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x005e, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x005f, "SL", "Increment Between Channels", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0060, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0061, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0062, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0063, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0064, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0065, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0066, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0067, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0068, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0069, "UL", "Convolution Mode", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x006a, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x006b, "SS", "Field Of View In Detector Cells", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x006c, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x006e, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0070, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0071, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0072, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0073, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0074, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0075, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0076, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0077, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0078, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x007a, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x007c, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x007d, "DS", "Second Echo", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x007e, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x007f, "DS", "Table Delta", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0080, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0081, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0082, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0083, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0084, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0085, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0086, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0087, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0088, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x008a, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x008b, "SS", "Actual Receive Gain Digital", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x008c, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x008d, "DS", "Delay After Trigger", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x008e, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x008f, "SS", "Swap Phase Frequency", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0090, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0091, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0092, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0093, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0094, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0095, "SS", "Analog Receiver Gain", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0096, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0097, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0098, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0099, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x009a, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x009b, "SS", "Pulse Sequence Mode", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x009c, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x009d, "DT", "Pulse Sequence Date", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x009e, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x009f, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00a0, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00a1, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00a2, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00a3, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00a4, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00a5, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00a6, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00a7, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00a8, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00a9, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00aa, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00ab, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00ac, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00ad, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00ae, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00af, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00b0, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00b1, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00b2, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00b3, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00b4, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00b5, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00b6, "DS", "User Data", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00b7, "DS", "User Data", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00b8, "DS", "User Data", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00b9, "DS", "User Data", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00ba, "DS", "User Data", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00bb, "DS", "User Data", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00bc, "DS", "User Data", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00bd, "DS", "User Data", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00be, "DS", "Projection Angle", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00c0, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00c1, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00c2, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00c3, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00c4, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00c5, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00c6, "SS", "SAT Location H", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00c7, "SS", "SAT Location F", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00c8, "SS", "SAT Thickness R L", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00c9, "SS", "SAT Thickness A P", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00ca, "SS", "SAT Thickness H F", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00cb, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00cc, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00cd, "SS", "Thickness Disclaimer", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00ce, "SS", "Prescan Type", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00cf, "SS", "Prescan Status", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00d0, "SH", "Raw Data Type", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00d1, "DS", "Flow Sensitivity", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00d2, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00d3, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00d4, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00d5, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00d6, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00d7, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00d8, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00d9, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00da, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00db, "DS", "Back Projector Coefficient", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00dc, "SS", "Primary Speed Correction Used", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00dd, "SS", "Overrange Correction Used", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00de, "DS", "Dynamic Z Alpha Value", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00df, "DS", "User Data", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00e0, "DS", "User Data", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00e1, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00e2, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00e3, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00e4, "LT", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00e5, "IS", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00e6, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00e8, "DS", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00e9, "DS", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00eb, "DS", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00ec, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00f0, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00f1, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00f2, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00f3, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00f4, "LT", "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00f9, "DS", "Transmission Gain", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x1015, "UN", "?", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0000, "UL", "Relationship Group Length", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x000d, "UI", "Study Instance UID", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x000e, "UI", "Series Instance UID", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0010, "SH", "Study ID", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0011, "IS", "Series Number", (DicomElemParseFunc *) funcDCM_SeriesNumber },
    { 0x0020, 0x0012, "IS", "Acquisition Number", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0013, "IS", "Instance (formerly Image) Number", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0014, "IS", "Isotope Number", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0015, "IS", "Phase Number", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0016, "IS", "Interval Number", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0017, "IS", "Time Slot Number", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0018, "IS", "Angle Number", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0020, "CS", "Patient Orientation", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0022, "IS", "Overlay Number", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0024, "IS", "Curve Number", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0026, "IS", "LUT Number", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0030, "DS", "Image Position", (DicomElemParseFunc *) funcDCM_ImagePosition },
    { 0x0020, 0x0032, "DS", "Image Position (Patient)", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0035, "DS", "Image Orientation", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0037, "DS", "Image Orientation (Patient)", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0050, "DS", "Location", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0052, "UI", "Frame of Reference UID", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0060, "CS", "Laterality", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0062, "CS", "Image Laterality", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0070, "LT", "Image Geometry Type", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0080, "LO", "Masking Image", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0100, "IS", "Temporal Position Identifier", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0105, "IS", "Number of Temporal Positions", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0110, "DS", "Temporal Resolution", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1000, "IS", "Series in Study", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1001, "DS", "Acquisitions in Series", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1002, "IS", "Images in Acquisition", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1003, "IS", "Images in Series", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1004, "IS", "Acquisitions in Study", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1005, "IS", "Images in Study", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1020, "LO", "Reference", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1040, "LO", "Position Reference Indicator", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1041, "DS", "Slice Location", (DicomElemParseFunc *) funcDCM_SliceLocation },
    { 0x0020, 0x1070, "IS", "Other Study Numbers", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1200, "IS", "Number of Patient Related Studies", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1202, "IS", "Number of Patient Related Series", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1204, "IS", "Number of Patient Related Images", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1206, "IS", "Number of Study Related Series", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1208, "IS", "Number of Study Related Series", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x3100, "LO", "Source Image IDs", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x3401, "LO", "Modifying Device ID", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x3402, "LO", "Modified Image ID", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x3403, "xs", "Modified Image Date", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x3404, "LO", "Modifying Device Manufacturer", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x3405, "xs", "Modified Image Time", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x3406, "xs", "Modified Image Description", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x4000, "LT", "Image Comments", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x5000, "AT", "Original Image Identification", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x5002, "LO", "Original Image Identification Nomenclature", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0000, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0001, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0002, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0003, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0004, "DS", "VOI Position", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0005, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0006, "IS", "CSI Matrix Size Original", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0007, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0008, "DS", "Spatial Grid Shift", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0009, "DS", "Signal Limits Minimum", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0010, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0011, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0012, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0013, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0014, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0015, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0016, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0017, "DS", "EPI Operation Mode Flag", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0018, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0019, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0020, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0021, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0022, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0024, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0025, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0026, "IS", "Image Pixel Offset", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0030, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0031, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0032, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0034, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0035, "SS", "Series From Which Prescribed", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0036, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0037, "SS", "Screen Format", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0039, "DS", "Slab Thickness", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0040, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0041, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0042, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0043, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0044, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0045, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0046, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0047, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0048, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0049, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x004a, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x004e, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x004f, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0050, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0051, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0052, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0053, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0054, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0055, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0056, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0057, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0058, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0059, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x005a, "SL", "Integer Slop", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x005b, "DS", "Float Slop", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x005c, "DS", "Float Slop", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x005d, "DS", "Float Slop", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x005e, "DS", "Float Slop", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x005f, "DS", "Float Slop", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0060, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0061, "DS", "Image Normal", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0062, "IS", "Reference Type Code", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0063, "DS", "Image Distance", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0065, "US", "Image Positioning History Mask", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x006a, "DS", "Image Row", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x006b, "DS", "Image Column", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0070, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0071, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0072, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0073, "DS", "Second Repetition Time", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0075, "DS", "Light Brightness", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0076, "DS", "Light Contrast", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x007a, "IS", "Overlay Threshold", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x007b, "IS", "Surface Threshold", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x007c, "IS", "Grey Scale Threshold", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0080, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0081, "DS", "Auto Window Level Alpha", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0082, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0083, "DS", "Auto Window Level Window", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0084, "DS", "Auto Window Level Level", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0090, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0091, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0092, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0093, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0094, "DS", "EPI Change Value of X Component", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0095, "DS", "EPI Change Value of Y Component", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0096, "DS", "EPI Change Value of Z Component", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x00a0, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x00a1, "DS", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x00a2, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x00a3, "LT", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x00a4, "LT", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x00a7, "LT", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x00b0, "IS", "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x00c0, "IS", "?", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x0000, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x0001, "SL", "Number Of Series In Study", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x0002, "SL", "Number Of Unarchived Series", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x0010, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x0020, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x0030, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x0040, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x0050, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x0060, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x0070, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x0074, "SL", "Number Of Updates To Info", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x007d, "SS", "Indicates If Study Has Complete Info", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x0080, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x0090, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x00ff, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0025, 0x0000, "UL", "Group Length", (DicomElemParseFunc *) NULL },
    { 0x0025, 0x0006, "SS", "Last Pulse Sequence Used", (DicomElemParseFunc *) NULL },
    { 0x0025, 0x0007, "SL", "Images In Series", (DicomElemParseFunc *) NULL },
    { 0x0025, 0x0010, "SS", "Landmark Counter", (DicomElemParseFunc *) NULL },
    { 0x0025, 0x0011, "SS", "Number Of Acquisitions", (DicomElemParseFunc *) NULL },
    { 0x0025, 0x0014, "SL", "Indicates Number Of Updates To Info", (DicomElemParseFunc *) NULL },
    { 0x0025, 0x0017, "SL", "Series Complete Flag", (DicomElemParseFunc *) NULL },
    { 0x0025, 0x0018, "SL", "Number Of Images Archived", (DicomElemParseFunc *) NULL },
    { 0x0025, 0x0019, "SL", "Last Image Number Used", (DicomElemParseFunc *) NULL },
    { 0x0025, 0x001a, "SH", "Primary Receiver Suite And Host", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0000, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0006, "SL", "Image Archive Flag", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0010, "SS", "Scout Type", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0011, "UN", "?", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0012, "IS", "?", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0013, "IS", "?", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0014, "IS", "?", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0015, "IS", "?", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0016, "LT", "?", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x001c, "SL", "Vma Mamp", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x001d, "SS", "Vma Phase", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x001e, "SL", "Vma Mod", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x001f, "SL", "Vma Clip", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0020, "SS", "Smart Scan On Off Flag", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0030, "SH", "Foreign Image Revision", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0031, "SS", "Imaging Mode", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0032, "SS", "Pulse Sequence", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0033, "SL", "Imaging Options", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0035, "SS", "Plane Type", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0036, "SL", "Oblique Plane", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0040, "SH", "RAS Letter Of Image Location", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0041, "FL", "Image Location", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0042, "FL", "Center R Coord Of Plane Image", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0043, "FL", "Center A Coord Of Plane Image", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0044, "FL", "Center S Coord Of Plane Image", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0045, "FL", "Normal R Coord", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0046, "FL", "Normal A Coord", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0047, "FL", "Normal S Coord", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0048, "FL", "R Coord Of Top Right Corner", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0049, "FL", "A Coord Of Top Right Corner", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x004a, "FL", "S Coord Of Top Right Corner", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x004b, "FL", "R Coord Of Bottom Right Corner", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x004c, "FL", "A Coord Of Bottom Right Corner", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x004d, "FL", "S Coord Of Bottom Right Corner", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0050, "FL", "Table Start Location", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0051, "FL", "Table End Location", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0052, "SH", "RAS Letter For Side Of Image", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0053, "SH", "RAS Letter For Anterior Posterior", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0054, "SH", "RAS Letter For Scout Start Loc", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0055, "SH", "RAS Letter For Scout End Loc", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0060, "FL", "Image Dimension X", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0061, "FL", "Image Dimension Y", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0062, "FL", "Number Of Excitations", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0000, "UL", "Image Presentation Group Length", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0002, "US", "Samples per Pixel", (DicomElemParseFunc *) funcDCM_SamplesPerPixel },
    { 0x0028, 0x0004, "CS", "Photometric Interpretation", (DicomElemParseFunc *) funcDCM_PhotometricInterpretation },
    { 0x0028, 0x0005, "US", "Image Dimensions", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0006, "US", "Planar Configuration", (DicomElemParseFunc *) funcDCM_PlanarConfiguration },
    { 0x0028, 0x0008, "IS", "Number of Frames", (DicomElemParseFunc *) funcDCM_NumberOfFrames },
    { 0x0028, 0x0009, "AT", "Frame Increment Pointer", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0010, "US", "Rows", (DicomElemParseFunc *) funcDCM_Rows },
    { 0x0028, 0x0011, "US", "Columns", (DicomElemParseFunc *) funcDCM_Columns },
    { 0x0028, 0x0012, "US", "Planes", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0014, "US", "Ultrasound Color Data Present", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0030, "DS", "Pixel Spacing", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0031, "DS", "Zoom Factor", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0032, "DS", "Zoom Center", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0034, "IS", "Pixel Aspect Ratio", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0040, "LO", "Image Format", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0050, "LT", "Manipulated Image", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0051, "CS", "Corrected Image", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x005f, "LO", "Compression Recognition Code", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0060, "LO", "Compression Code", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0061, "SH", "Compression Originator", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0062, "SH", "Compression Label", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0063, "SH", "Compression Description", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0065, "LO", "Compression Sequence", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0066, "AT", "Compression Step Pointers", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0068, "US", "Repeat Interval", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0069, "US", "Bits Grouped", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0070, "US", "Perimeter Table", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0071, "xs", "Perimeter Value", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0080, "US", "Predictor Rows", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0081, "US", "Predictor Columns", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0082, "US", "Predictor Constants", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0090, "LO", "Blocked Pixels", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0091, "US", "Block Rows", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0092, "US", "Block Columns", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0093, "US", "Row Overlap", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0094, "US", "Column Overlap", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0100, "US", "Bits Allocated", (DicomElemParseFunc *) funcDCM_BitsAllocated },
    { 0x0028, 0x0101, "US", "Bits Stored", (DicomElemParseFunc *) funcDCM_BitsStored },
    { 0x0028, 0x0102, "US", "High Bit", (DicomElemParseFunc *) funcDCM_HighBit },
    { 0x0028, 0x0103, "US", "Pixel Representation", (DicomElemParseFunc *) funcDCM_PixelRepresentation },
    { 0x0028, 0x0104, "xs", "Smallest Valid Pixel Value", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0105, "xs", "Largest Valid Pixel Value", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0106, "xs", "Smallest Image Pixel Value", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0107, "xs", "Largest Image Pixel Value", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0108, "xs", "Smallest Pixel Value in Series", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0109, "xs", "Largest Pixel Value in Series", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0110, "xs", "Smallest Pixel Value in Plane", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0111, "xs", "Largest Pixel Value in Plane", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0120, "xs", "Pixel Padding Value", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0121, "xs", "Pixel Padding Range Limit", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0200, "xs", "Image Location", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0300, "CS", "Quality Control Image", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0301, "CS", "Burned In Annotation", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0400, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0401, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0402, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0403, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0404, "AT", "Details of Coefficients", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0700, "LO", "DCT Label", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0701, "LO", "Data Block Description", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0702, "AT", "Data Block", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0710, "US", "Normalization Factor Format", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0720, "US", "Zonal Map Number Format", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0721, "AT", "Zonal Map Location", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0722, "US", "Zonal Map Format", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0730, "US", "Adaptive Map Format", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0740, "US", "Code Number Format", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0800, "LO", "Code Label", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0802, "US", "Number of Tables", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0803, "AT", "Code Table Location", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0804, "US", "Bits For Code Word", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0808, "AT", "Image Data Location", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1040, "CS", "Pixel Intensity Relationship", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1041, "SS", "Pixel Intensity Relationship Sign", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1050, "DS", "Window Center", (DicomElemParseFunc *) funcDCM_WindowCenter },
    { 0x0028, 0x1051, "DS", "Window Width", (DicomElemParseFunc *) funcDCM_WindowWidth },
    { 0x0028, 0x1052, "DS", "Rescale Intercept", (DicomElemParseFunc *) funcDCM_RescaleIntercept },
    { 0x0028, 0x1053, "DS", "Rescale Slope", (DicomElemParseFunc *) funcDCM_RescaleSlope },
    { 0x0028, 0x1054, "LO", "Rescale Type", (DicomElemParseFunc *) funcDCM_RescaleType },
    { 0x0028, 0x1055, "LO", "Window Center & Width Explanation", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1080, "LO", "Gray Scale", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1090, "CS", "Recommended Viewing Mode", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1100, "xs", "Gray Lookup Table Descriptor", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1101, "xs", "Red Palette Color Lookup Table Descriptor", (DicomElemParseFunc *) funcDCM_PaletteDescriptor },
    { 0x0028, 0x1102, "xs", "Green Palette Color Lookup Table Descriptor", (DicomElemParseFunc *) funcDCM_PaletteDescriptor },
    { 0x0028, 0x1103, "xs", "Blue Palette Color Lookup Table Descriptor", (DicomElemParseFunc *) funcDCM_PaletteDescriptor },
    { 0x0028, 0x1111, "OW", "Large Red Palette Color Lookup Table Descriptor", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1112, "OW", "Large Green Palette Color Lookup Table Descriptor", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1113, "OW", "Large Blue Palette Color Lookup Table Descriptor", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1199, "UI", "Palette Color Lookup Table UID", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1200, "xs", "Gray Lookup Table Data", (DicomElemParseFunc *) funcDCM_LUT },
    { 0x0028, 0x1201, "OW", "Red Palette Color Lookup Table Data", (DicomElemParseFunc *) funcDCM_Palette },
    { 0x0028, 0x1202, "OW", "Green Palette Color Lookup Table Data", (DicomElemParseFunc *) funcDCM_Palette },
    { 0x0028, 0x1203, "OW", "Blue Palette Color Lookup Table Data", (DicomElemParseFunc *) funcDCM_Palette },
    { 0x0028, 0x1211, "OW", "Large Red Palette Color Lookup Table Data", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1212, "OW", "Large Green Palette Color Lookup Table Data", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1213, "OW", "Large Blue Palette Color Lookup Table Data", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1214, "UI", "Large Palette Color Lookup Table UID", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1221, "OW", "Segmented Red Palette Color Lookup Table Data", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1222, "OW", "Segmented Green Palette Color Lookup Table Data", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1223, "OW", "Segmented Blue Palette Color Lookup Table Data", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1300, "CS", "Implant Present", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x2110, "CS", "Lossy Image Compression", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x2112, "DS", "Lossy Image Compression Ratio", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x3000, "SQ", "Modality LUT Sequence", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x3002, "US", "LUT Descriptor", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x3003, "LO", "LUT Explanation", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x3004, "LO", "Modality LUT Type", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x3006, "US", "LUT Data", (DicomElemParseFunc *) funcDCM_LUT },
    { 0x0028, 0x3010, "xs", "VOI LUT Sequence", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x4000, "LT", "Image Presentation Comments", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x5000, "SQ", "Biplane Acquisition Sequence", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x6010, "US", "Representative Frame Number", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x6020, "US", "Frame Numbers of Interest", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x6022, "LO", "Frame of Interest Description", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x6030, "US", "Mask Pointer", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x6040, "US", "R Wave Pointer", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x6100, "SQ", "Mask Subtraction Sequence", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x6101, "CS", "Mask Operation", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x6102, "US", "Applicable Frame Range", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x6110, "US", "Mask Frame Numbers", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x6112, "US", "Contrast Frame Averaging", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x6114, "FL", "Mask Sub-Pixel Shift", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x6120, "SS", "TID Offset", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x6190, "ST", "Mask Operation Explanation", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0000, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0001, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0002, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0003, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0004, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0005, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0006, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0007, "SL", "Lower Range Of Pixels", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0008, "SH", "Lower Range Of Pixels", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0009, "SH", "Lower Range Of Pixels", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x000a, "SS", "Lower Range Of Pixels", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x000c, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x000e, "CS", "Zoom Enable Status", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x000f, "CS", "Zoom Select Status", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0010, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0011, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0013, "LT", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0015, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0016, "SL", "Lower Range Of Pixels", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0017, "SL", "Lower Range Of Pixels", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0018, "SL", "Upper Range Of Pixels", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x001a, "SL", "Length Of Total Info In Bytes", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x001e, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x001f, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0020, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0022, "IS", "Pixel Quality Value", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0025, "LT", "Processed Pixel Data Quality", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0026, "SS", "Version Of Info Structure", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0030, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0031, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0032, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0033, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0034, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0035, "SL", "Advantage Comp Underflow", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0038, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0040, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0041, "DS", "Magnifying Glass Rectangle", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0043, "DS", "Magnifying Glass Factor", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0044, "US", "Magnifying Glass Function", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x004e, "CS", "Magnifying Glass Enable Status", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x004f, "CS", "Magnifying Glass Select Status", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0050, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0051, "LT", "Exposure Code", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0052, "LT", "Sort Code", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0053, "LT", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0060, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0061, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0067, "LT", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0070, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0071, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0072, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0077, "CS", "Window Select Status", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0078, "LT", "ECG Display Printing ID", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0079, "CS", "ECG Display Printing", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x007e, "CS", "ECG Display Printing Enable Status", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x007f, "CS", "ECG Display Printing Select Status", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0080, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0081, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0082, "IS", "View Zoom", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0083, "IS", "View Transform", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x008e, "CS", "Physiological Display Enable Status", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x008f, "CS", "Physiological Display Select Status", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0090, "IS", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0099, "LT", "Shutter Type", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00a0, "US", "Rows of Rectangular Shutter", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00a1, "US", "Columns of Rectangular Shutter", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00a2, "US", "Origin of Rectangular Shutter", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00b0, "US", "Radius of Circular Shutter", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00b2, "US", "Origin of Circular Shutter", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00c0, "LT", "Functional Shutter ID", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00c1, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00c3, "IS", "Scan Resolution", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00c4, "IS", "Field of View", (DicomElemParseFunc *) funcDCM_FieldOfView },
    { 0x0029, 0x00c5, "LT", "Field Of Shutter Rectangle", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00ce, "CS", "Shutter Enable Status", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00cf, "CS", "Shutter Select Status", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00d0, "IS", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00d1, "IS", "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00d5, "LT", "Slice Thickness", (DicomElemParseFunc *) NULL },
    { 0x0031, 0x0010, "LT", "Request UID", (DicomElemParseFunc *) NULL },
    { 0x0031, 0x0012, "LT", "Examination Reason", (DicomElemParseFunc *) NULL },
    { 0x0031, 0x0030, "DA", "Requested Date", (DicomElemParseFunc *) NULL },
    { 0x0031, 0x0032, "TM", "Worklist Request Start Time", (DicomElemParseFunc *) NULL },
    { 0x0031, 0x0033, "TM", "Worklist Request End Time", (DicomElemParseFunc *) NULL },
    { 0x0031, 0x0045, "LT", "Requesting Physician", (DicomElemParseFunc *) NULL },
    { 0x0031, 0x004a, "TM", "Requested Time", (DicomElemParseFunc *) NULL },
    { 0x0031, 0x0050, "LT", "Requested Physician", (DicomElemParseFunc *) NULL },
    { 0x0031, 0x0080, "LT", "Requested Location", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x0000, "UL", "Study Group Length", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x000a, "CS", "Study Status ID", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x000c, "CS", "Study Priority ID", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x0012, "LO", "Study ID Issuer", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x0032, "DA", "Study Verified Date", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x0033, "TM", "Study Verified Time", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x0034, "DA", "Study Read Date", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x0035, "TM", "Study Read Time", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1000, "DA", "Scheduled Study Start Date", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1001, "TM", "Scheduled Study Start Time", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1010, "DA", "Scheduled Study Stop Date", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1011, "TM", "Scheduled Study Stop Time", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1020, "LO", "Scheduled Study Location", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1021, "AE", "Scheduled Study Location AE Title(s)", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1030, "LO", "Reason for Study", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1032, "PN", "Requesting Physician", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1033, "LO", "Requesting Service", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1040, "DA", "Study Arrival Date", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1041, "TM", "Study Arrival Time", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1050, "DA", "Study Completion Date", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1051, "TM", "Study Completion Time", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1055, "CS", "Study Component Status ID", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1060, "LO", "Requested Procedure Description", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1064, "SQ", "Requested Procedure Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1070, "LO", "Requested Contrast Agent", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x4000, "LT", "Study Comments", (DicomElemParseFunc *) NULL },
    { 0x0033, 0x0001, "UN", "?", (DicomElemParseFunc *) NULL },
    { 0x0033, 0x0002, "UN", "?", (DicomElemParseFunc *) NULL },
    { 0x0033, 0x0005, "UN", "?", (DicomElemParseFunc *) NULL },
    { 0x0033, 0x0006, "UN", "?", (DicomElemParseFunc *) NULL },
    { 0x0033, 0x0010, "LT", "Patient Study UID", (DicomElemParseFunc *) NULL },
    { 0x0037, 0x0010, "LO", "ReferringDepartment", (DicomElemParseFunc *) NULL },
    { 0x0037, 0x0020, "US", "ScreenNumber", (DicomElemParseFunc *) NULL },
    { 0x0037, 0x0040, "SH", "LeftOrientation", (DicomElemParseFunc *) NULL },
    { 0x0037, 0x0042, "SH", "RightOrientation", (DicomElemParseFunc *) NULL },
    { 0x0037, 0x0050, "CS", "Inversion", (DicomElemParseFunc *) NULL },
    { 0x0037, 0x0060, "US", "DSA", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0000, "UL", "Visit Group Length", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0004, "SQ", "Referenced Patient Alias Sequence", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0008, "CS", "Visit Status ID", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0010, "LO", "Admission ID", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0011, "LO", "Issuer of Admission ID", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0016, "LO", "Route of Admissions", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x001a, "DA", "Scheduled Admission Date", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x001b, "TM", "Scheduled Admission Time", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x001c, "DA", "Scheduled Discharge Date", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x001d, "TM", "Scheduled Discharge Time", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x001e, "LO", "Scheduled Patient Institution Residence", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0020, "DA", "Admitting Date", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0021, "TM", "Admitting Time", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0030, "DA", "Discharge Date", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0032, "TM", "Discharge Time", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0040, "LO", "Discharge Diagnosis Description", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0044, "SQ", "Discharge Diagnosis Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0050, "LO", "Special Needs", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0300, "LO", "Current Patient Location", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0400, "LO", "Patient's Institution Residence", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0500, "LO", "Patient State", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x4000, "LT", "Visit Comments", (DicomElemParseFunc *) NULL },
    { 0x0039, 0x0080, "IS", "Private Entity Number", (DicomElemParseFunc *) NULL },
    { 0x0039, 0x0085, "DA", "Private Entity Date", (DicomElemParseFunc *) NULL },
    { 0x0039, 0x0090, "TM", "Private Entity Time", (DicomElemParseFunc *) NULL },
    { 0x0039, 0x0095, "LO", "Private Entity Launch Command", (DicomElemParseFunc *) NULL },
    { 0x0039, 0x00aa, "CS", "Private Entity Type", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0002, "SQ", "Waveform Sequence", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0005, "US", "Waveform Number of Channels", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0010, "UL", "Waveform Number of Samples", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x001a, "DS", "Sampling Frequency", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0020, "SH", "Group Label", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0103, "CS", "Waveform Sample Value Representation", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0122, "OB", "Waveform Padding Value", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0200, "SQ", "Channel Definition", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0202, "IS", "Waveform Channel Number", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0203, "SH", "Channel Label", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0205, "CS", "Channel Status", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0208, "SQ", "Channel Source", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0209, "SQ", "Channel Source Modifiers", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x020a, "SQ", "Differential Channel Source", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x020b, "SQ", "Differential Channel Source Modifiers", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0210, "DS", "Channel Sensitivity", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0211, "SQ", "Channel Sensitivity Units", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0212, "DS", "Channel Sensitivity Correction Factor", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0213, "DS", "Channel Baseline", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0214, "DS", "Channel Time Skew", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0215, "DS", "Channel Sample Skew", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0216, "OB", "Channel Minimum Value", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0217, "OB", "Channel Maximum Value", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0218, "DS", "Channel Offset", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x021a, "US", "Bits Per Sample", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0220, "DS", "Filter Low Frequency", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0221, "DS", "Filter High Frequency", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0222, "DS", "Notch Filter Frequency", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0223, "DS", "Notch Filter Bandwidth", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x1000, "OB", "Waveform Data", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0001, "AE", "Scheduled Station AE Title", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0002, "DA", "Scheduled Procedure Step Start Date", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0003, "TM", "Scheduled Procedure Step Start Time", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0004, "DA", "Scheduled Procedure Step End Date", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0005, "TM", "Scheduled Procedure Step End Time", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0006, "PN", "Scheduled Performing Physician Name", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0007, "LO", "Scheduled Procedure Step Description", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0008, "SQ", "Scheduled Action Item Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0009, "SH", "Scheduled Procedure Step ID", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0010, "SH", "Scheduled Station Name", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0011, "SH", "Scheduled Procedure Step Location", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0012, "LO", "Pre-Medication", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0020, "CS", "Scheduled Procedure Step Status", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0100, "SQ", "Scheduled Procedure Step Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0302, "US", "Entrance Dose", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0303, "US", "Exposed Area", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0306, "DS", "Distance Source to Entrance", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0307, "DS", "Distance Source to Support", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0310, "ST", "Comments On Radiation Dose", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0312, "DS", "X-Ray Output", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0314, "DS", "Half Value Layer", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0316, "DS", "Organ Dose", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0318, "CS", "Organ Exposed", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0400, "LT", "Comments On Scheduled Procedure Step", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x050a, "LO", "Specimen Accession Number", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0550, "SQ", "Specimen Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0551, "LO", "Specimen Identifier", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0552, "SQ", "Specimen Description Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0553, "ST", "Specimen Description", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0555, "SQ", "Acquisition Context Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0556, "ST", "Acquisition Context Description", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x059a, "SQ", "Specimen Type Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x06fa, "LO", "Slide Identifier", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x071a, "SQ", "Image Center Point Coordinates Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x072a, "DS", "X Offset In Slide Coordinate System", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x073a, "DS", "Y Offset In Slide Coordinate System", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x074a, "DS", "Z Offset In Slide Coordinate System", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x08d8, "SQ", "Pixel Spacing Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x08da, "SQ", "Coordinate System Axis Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x08ea, "SQ", "Measurement Units Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x09f8, "SQ", "Vital Stain Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x1001, "SH", "Requested Procedure ID", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x1002, "LO", "Reason For Requested Procedure", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x1003, "SH", "Requested Procedure Priority", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x1004, "LO", "Patient Transport Arrangements", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x1005, "LO", "Requested Procedure Location", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x1006, "SH", "Placer Order Number of Procedure", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x1007, "SH", "Filler Order Number of Procedure", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x1008, "LO", "Confidentiality Code", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x1009, "SH", "Reporting Priority", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x1010, "PN", "Names of Intended Recipients of Results", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x1400, "LT", "Requested Procedure Comments", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x2001, "LO", "Reason For Imaging Service Request", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x2004, "DA", "Issue Date of Imaging Service Request", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x2005, "TM", "Issue Time of Imaging Service Request", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x2006, "SH", "Placer Order Number of Imaging Service Request", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x2007, "SH", "Filler Order Number of Imaging Service Request", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x2008, "PN", "Order Entered By", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x2009, "SH", "Order Enterer Location", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x2010, "SH", "Order Callback Phone Number", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x2400, "LT", "Imaging Service Request Comments", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x3001, "LO", "Confidentiality Constraint On Patient Data", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa007, "CS", "Findings Flag", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa020, "SQ", "Findings Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa021, "UI", "Findings Group UID", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa022, "UI", "Referenced Findings Group UID", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa023, "DA", "Findings Group Recording Date", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa024, "TM", "Findings Group Recording Time", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa026, "SQ", "Findings Source Category Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa027, "LO", "Documenting Organization", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa028, "SQ", "Documenting Organization Identifier Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa032, "LO", "History Reliability Qualifier Description", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa043, "SQ", "Concept Name Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa047, "LO", "Measurement Precision Description", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa057, "CS", "Urgency or Priority Alerts", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa060, "LO", "Sequencing Indicator", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa066, "SQ", "Document Identifier Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa067, "PN", "Document Author", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa068, "SQ", "Document Author Identifier Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa070, "SQ", "Identifier Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa073, "LO", "Object String Identifier", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa074, "OB", "Object Binary Identifier", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa075, "PN", "Documenting Observer", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa076, "SQ", "Documenting Observer Identifier Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa078, "SQ", "Observation Subject Identifier Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa080, "SQ", "Person Identifier Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa085, "SQ", "Procedure Identifier Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa088, "LO", "Object Directory String Identifier", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa089, "OB", "Object Directory Binary Identifier", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa090, "CS", "History Reliability Qualifier", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa0a0, "CS", "Referenced Type of Data", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa0b0, "US", "Referenced Waveform Channels", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa110, "DA", "Date of Document or Verbal Transaction", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa112, "TM", "Time of Document Creation or Verbal Transaction", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa121, "DA", "Date", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa122, "TM", "Time", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa123, "PN", "Person Name", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa124, "SQ", "Referenced Person Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa125, "CS", "Report Status ID", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa130, "CS", "Temporal Range Type", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa132, "UL", "Referenced Sample Offsets", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa136, "US", "Referenced Frame Numbers", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa138, "DS", "Referenced Time Offsets", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa13a, "DT", "Referenced Datetime", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa160, "UT", "Text Value", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa167, "SQ", "Observation Category Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa168, "SQ", "Concept Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa16a, "ST", "Bibliographic Citation", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa170, "CS", "Observation Class", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa171, "UI", "Observation UID", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa172, "UI", "Referenced Observation UID", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa173, "CS", "Referenced Observation Class", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa174, "CS", "Referenced Object Observation Class", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa180, "US", "Annotation Group Number", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa192, "DA", "Observation Date", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa193, "TM", "Observation Time", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa194, "CS", "Measurement Automation", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa195, "SQ", "Concept Name Code Sequence Modifier", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa224, "ST", "Identification Description", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa290, "CS", "Coordinates Set Geometric Type", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa296, "SQ", "Algorithm Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa297, "ST", "Algorithm Description", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa29a, "SL", "Pixel Coordinates Set", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa300, "SQ", "Measured Value Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa307, "PN", "Current Observer", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa30a, "DS", "Numeric Value", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa313, "SQ", "Referenced Accession Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa33a, "ST", "Report Status Comment", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa340, "SQ", "Procedure Context Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa352, "PN", "Verbal Source", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa353, "ST", "Address", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa354, "LO", "Telephone Number", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa358, "SQ", "Verbal Source Identifier Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa380, "SQ", "Report Detail Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa402, "UI", "Observation Subject UID", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa403, "CS", "Observation Subject Class", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa404, "SQ", "Observation Subject Type Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa600, "CS", "Observation Subject Context Flag", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa601, "CS", "Observer Context Flag", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa603, "CS", "Procedure Context Flag", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa730, "SQ", "Observations Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa731, "SQ", "Relationship Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa732, "SQ", "Relationship Type Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa744, "SQ", "Language Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa992, "ST", "Uniform Resource Locator", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xb020, "SQ", "Annotation Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xdb73, "SQ", "Relationship Type Code Sequence Modifier", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0000, "LT", "Papyrus Comments", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0010, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0011, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0012, "UL", "Pixel Offset", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0013, "SQ", "Image Identifier Sequence", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0014, "SQ", "External File Reference Sequence", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0015, "US", "Number of Images", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0020, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0021, "UI", "Referenced SOP Class UID", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0022, "UI", "Referenced SOP Instance UID", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0030, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0031, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0032, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0034, "DA", "Modified Date", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0036, "TM", "Modified Time", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0040, "LT", "Owner Name", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0041, "UI", "Referenced Image SOP Class UID", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0042, "UI", "Referenced Image SOP Instance UID", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0050, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0060, "UL", "Number of Images", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0062, "UL", "Number of Other", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x00a0, "LT", "External Folder Element DSID", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x00a1, "US", "External Folder Element Data Set Type", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x00a2, "LT", "External Folder Element File Location", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x00a3, "UL", "External Folder Element Length", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x00b0, "LT", "Internal Folder Element DSID", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x00b1, "US", "Internal Folder Element Data Set Type", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x00b2, "UL", "Internal Offset To Data Set", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x00b3, "UL", "Internal Offset To Image", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0001, "SS", "Bitmap Of Prescan Options", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0002, "SS", "Gradient Offset In X", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0003, "SS", "Gradient Offset In Y", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0004, "SS", "Gradient Offset In Z", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0005, "SS", "Image Is Original Or Unoriginal", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0006, "SS", "Number Of EPI Shots", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0007, "SS", "Views Per Segment", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0008, "SS", "Respiratory Rate In BPM", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0009, "SS", "Respiratory Trigger Point", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x000a, "SS", "Type Of Receiver Used", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x000b, "DS", "Peak Rate Of Change Of Gradient Field", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x000c, "DS", "Limits In Units Of Percent", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x000d, "DS", "PSD Estimated Limit", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x000e, "DS", "PSD Estimated Limit In Tesla Per Second", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x000f, "DS", "SAR Avg Head", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0010, "US", "Window Value", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0011, "US", "Total Input Views", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0012, "SS", "Xray Chain", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0013, "SS", "Recon Kernel Parameters", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0014, "SS", "Calibration Parameters", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0015, "SS", "Total Output Views", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0016, "SS", "Number Of Overranges", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0017, "DS", "IBH Image Scale Factors", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0018, "DS", "BBH Coefficients", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0019, "SS", "Number Of BBH Chains To Blend", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x001a, "SL", "Starting Channel Number", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x001b, "SS", "PPScan Parameters", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x001c, "SS", "GE Image Integrity", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x001d, "SS", "Level Value", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x001e, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x001f, "SL", "Max Overranges In A View", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0020, "DS", "Avg Overranges All Views", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0021, "SS", "Corrected Afterglow Terms", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0025, "SS", "Reference Channels", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0026, "US", "No Views Ref Channels Blocked", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0027, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0028, "OB", "Unique Image Identifier", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0029, "OB", "Histogram Tables", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x002a, "OB", "User Defined Data", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x002b, "SS", "Private Scan Options", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x002c, "SS", "Effective Echo Spacing", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x002d, "SH", "String Slop Field 1", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x002e, "SH", "String Slop Field 2", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x002f, "SS", "Raw Data Type", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0030, "SS", "Raw Data Type", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0031, "DS", "RA Coord Of Target Recon Centre", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0032, "SS", "Raw Data Type", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0033, "FL", "Neg Scan Spacing", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0034, "IS", "Offset Frequency", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0035, "UL", "User Usage Tag", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0036, "UL", "User Fill Map MSW", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0037, "UL", "User Fill Map LSW", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0038, "FL", "User 25 To User 48", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0039, "IS", "Slop Integer 6 To Slop Integer 9", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0040, "FL", "Trigger On Position", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0041, "FL", "Degree Of Rotation", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0042, "SL", "DAS Trigger Source", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0043, "SL", "DAS Fpa Gain", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0044, "SL", "DAS Output Source", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0045, "SL", "DAS Ad Input", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0046, "SL", "DAS Cal Mode", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0047, "SL", "DAS Cal Frequency", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0048, "SL", "DAS Reg Xm", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0049, "SL", "DAS Auto Zero", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x004a, "SS", "Starting Channel Of View", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x004b, "SL", "DAS Xm Pattern", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x004c, "SS", "TGGC Trigger Mode", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x004d, "FL", "Start Scan To Xray On Delay", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x004e, "FL", "Duration Of Xray On", (DicomElemParseFunc *) NULL },
    { 0x0044, 0x0000, "UI", "?", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0004, "CS", "AES", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0006, "DS", "Angulation", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0009, "DS", "Real Magnification Factor", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x000b, "CS", "Senograph Type", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x000c, "DS", "Integration Time", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x000d, "DS", "ROI Origin X and Y", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0011, "DS", "Receptor Size cm X and Y", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0012, "IS", "Receptor Size Pixels X and Y", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0013, "ST", "Screen", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0014, "DS", "Pixel Pitch Microns", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0015, "IS", "Pixel Depth Bits", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0016, "IS", "Binning Factor X and Y", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x001b, "CS", "Clinical View", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x001d, "DS", "Mean Of Raw Gray Levels", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x001e, "DS", "Mean Of Offset Gray Levels", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x001f, "DS", "Mean Of Corrected Gray Levels", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0020, "DS", "Mean Of Region Gray Levels", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0021, "DS", "Mean Of Log Region Gray Levels", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0022, "DS", "Standard Deviation Of Raw Gray Levels", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0023, "DS", "Standard Deviation Of Corrected Gray Levels", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0024, "DS", "Standard Deviation Of Region Gray Levels", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0025, "DS", "Standard Deviation Of Log Region Gray Levels", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0026, "OB", "MAO Buffer", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0027, "IS", "Set Number", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0028, "CS", "WindowingType (LINEAR or GAMMA)", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0029, "DS", "WindowingParameters", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x002a, "IS", "Crosshair Cursor X Coordinates", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x002b, "IS", "Crosshair Cursor Y Coordinates", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0039, "US", "Vignette Rows", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x003a, "US", "Vignette Columns", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x003b, "US", "Vignette Bits Allocated", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x003c, "US", "Vignette Bits Stored", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x003d, "US", "Vignette High Bit", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x003e, "US", "Vignette Pixel Representation", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x003f, "OB", "Vignette Pixel Data", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0001, "SQ", "Reconstruction Parameters Sequence", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0050, "UL", "Volume Voxel Count", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0051, "UL", "Volume Segment Count", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0053, "US", "Volume Slice Size", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0054, "US", "Volume Slice Count", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0055, "SL", "Volume Threshold Value", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0057, "DS", "Volume Voxel Ratio", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0058, "DS", "Volume Voxel Size", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0059, "US", "Volume Z Position Size", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0060, "DS", "Volume Base Line", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0061, "DS", "Volume Center Point", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0063, "SL", "Volume Skew Base", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0064, "DS", "Volume Registration Transform Rotation Matrix", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0065, "DS", "Volume Registration Transform Translation Vector", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0070, "DS", "KVP List", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0071, "IS", "XRay Tube Current List", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0072, "IS", "Exposure List", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0080, "LO", "Acquisition DLX Identifier", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0085, "SQ", "Acquisition DLX 2D Series Sequence", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0089, "DS", "Contrast Agent Volume List", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x008a, "US", "Number Of Injections", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x008b, "US", "Frame Count", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0096, "IS", "Used Frames", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0091, "LO", "XA 3D Reconstruction Algorithm Name", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0092, "CS", "XA 3D Reconstruction Algorithm Version", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0093, "DA", "DLX Calibration Date", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0094, "TM", "DLX Calibration Time", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0095, "CS", "DLX Calibration Status", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0098, "US", "Transform Count", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0099, "SQ", "Transform Sequence", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x009a, "DS", "Transform Rotation Matrix", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x009b, "DS", "Transform Translation Vector", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x009c, "LO", "Transform Label", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00b1, "US", "Wireframe Count", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00b2, "US", "Location System", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00b0, "SQ", "Wireframe List", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00b5, "LO", "Wireframe Name", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00b6, "LO", "Wireframe Group Name", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00b7, "LO", "Wireframe Color", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00b8, "SL", "Wireframe Attributes", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00b9, "SL", "Wireframe Point Count", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00ba, "SL", "Wireframe Timestamp", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00bb, "SQ", "Wireframe Point List", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00bc, "DS", "Wireframe Points Coordinates", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00c0, "DS", "Volume Upper Left High Corner RAS", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00c1, "DS", "Volume Slice To RAS Rotation Matrix", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00c2, "DS", "Volume Upper Left High Corner TLOC", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00d1, "OB", "Volume Segment List", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00d2, "OB", "Volume Gradient List", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00d3, "OB", "Volume Density List", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00d4, "OB", "Volume Z Position List", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00d5, "OB", "Volume Original Index List", (DicomElemParseFunc *) NULL },
    { 0x0050, 0x0000, "UL", "Calibration Group Length", (DicomElemParseFunc *) NULL },
    { 0x0050, 0x0004, "CS", "Calibration Object", (DicomElemParseFunc *) NULL },
    { 0x0050, 0x0010, "SQ", "DeviceSequence", (DicomElemParseFunc *) NULL },
    { 0x0050, 0x0014, "DS", "DeviceLength", (DicomElemParseFunc *) NULL },
    { 0x0050, 0x0016, "DS", "DeviceDiameter", (DicomElemParseFunc *) NULL },
    { 0x0050, 0x0017, "CS", "DeviceDiameterUnits", (DicomElemParseFunc *) NULL },
    { 0x0050, 0x0018, "DS", "DeviceVolume", (DicomElemParseFunc *) NULL },
    { 0x0050, 0x0019, "DS", "InterMarkerDistance", (DicomElemParseFunc *) NULL },
    { 0x0050, 0x0020, "LO", "DeviceDescription", (DicomElemParseFunc *) NULL },
    { 0x0050, 0x0030, "SQ", "CodedInterventionDeviceSequence", (DicomElemParseFunc *) NULL },
    { 0x0051, 0x0010, "xs", "Image Text", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0000, "UL", "Nuclear Acquisition Group Length", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0010, "US", "Energy Window Vector", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0011, "US", "Number of Energy Windows", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0012, "SQ", "Energy Window Information Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0013, "SQ", "Energy Window Range Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0014, "DS", "Energy Window Lower Limit", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0015, "DS", "Energy Window Upper Limit", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0016, "SQ", "Radiopharmaceutical Information Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0017, "IS", "Residual Syringe Counts", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0018, "SH", "Energy Window Name", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0020, "US", "Detector Vector", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0021, "US", "Number of Detectors", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0022, "SQ", "Detector Information Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0030, "US", "Phase Vector", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0031, "US", "Number of Phases", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0032, "SQ", "Phase Information Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0033, "US", "Number of Frames In Phase", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0036, "IS", "Phase Delay", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0038, "IS", "Pause Between Frames", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0050, "US", "Rotation Vector", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0051, "US", "Number of Rotations", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0052, "SQ", "Rotation Information Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0053, "US", "Number of Frames In Rotation", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0060, "US", "R-R Interval Vector", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0061, "US", "Number of R-R Intervals", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0062, "SQ", "Gated Information Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0063, "SQ", "Data Information Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0070, "US", "Time Slot Vector", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0071, "US", "Number of Time Slots", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0072, "SQ", "Time Slot Information Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0073, "DS", "Time Slot Time", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0080, "US", "Slice Vector", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0081, "US", "Number of Slices", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0090, "US", "Angular View Vector", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0100, "US", "Time Slice Vector", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0101, "US", "Number Of Time Slices", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0200, "DS", "Start Angle", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0202, "CS", "Type of Detector Motion", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0210, "IS", "Trigger Vector", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0211, "US", "Number of Triggers in Phase", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0220, "SQ", "View Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0222, "SQ", "View Modifier Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0300, "SQ", "Radionuclide Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0302, "SQ", "Radiopharmaceutical Route Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0304, "SQ", "Radiopharmaceutical Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0306, "SQ", "Calibration Data Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0308, "US", "Energy Window Number", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0400, "SH", "Image ID", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0410, "SQ", "Patient Orientation Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0412, "SQ", "Patient Orientation Modifier Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0414, "SQ", "Patient Gantry Relationship Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1000, "CS", "Positron Emission Tomography Series Type", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1001, "CS", "Positron Emission Tomography Units", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1002, "CS", "Counts Source", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1004, "CS", "Reprojection Method", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1100, "CS", "Randoms Correction Method", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1101, "LO", "Attenuation Correction Method", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1102, "CS", "Decay Correction", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1103, "LO", "Reconstruction Method", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1104, "LO", "Detector Lines of Response Used", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1105, "LO", "Scatter Correction Method", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1200, "DS", "Axial Acceptance", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1201, "IS", "Axial Mash", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1202, "IS", "Transverse Mash", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1203, "DS", "Detector Element Size", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1210, "DS", "Coincidence Window Width", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1220, "CS", "Secondary Counts Type", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1300, "DS", "Frame Reference Time", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1310, "IS", "Primary Prompts Counts Accumulated", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1311, "IS", "Secondary Counts Accumulated", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1320, "DS", "Slice Sensitivity Factor", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1321, "DS", "Decay Factor", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1322, "DS", "Dose Calibration Factor", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1323, "DS", "Scatter Fraction Factor", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1324, "DS", "Dead Time Factor", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1330, "US", "Image Index", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1400, "CS", "Counts Included", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1401, "CS", "Dead Time Correction Flag", (DicomElemParseFunc *) NULL },
    { 0x0055, 0x0046, "LT", "Current Ward", (DicomElemParseFunc *) NULL },
    { 0x0058, 0x0000, "SQ", "?", (DicomElemParseFunc *) NULL },
    { 0x0060, 0x3000, "SQ", "Histogram Sequence", (DicomElemParseFunc *) NULL },
    { 0x0060, 0x3002, "US", "Histogram Number of Bins", (DicomElemParseFunc *) NULL },
    { 0x0060, 0x3004, "xs", "Histogram First Bin Value", (DicomElemParseFunc *) NULL },
    { 0x0060, 0x3006, "xs", "Histogram Last Bin Value", (DicomElemParseFunc *) NULL },
    { 0x0060, 0x3008, "US", "Histogram Bin Width", (DicomElemParseFunc *) NULL },
    { 0x0060, 0x3010, "LO", "Histogram Explanation", (DicomElemParseFunc *) NULL },
    { 0x0060, 0x3020, "UL", "Histogram Data", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0001, "SQ", "Graphic Annotation Sequence", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0002, "CS", "Graphic Layer", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0003, "CS", "Bounding Box Annotation Units", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0004, "CS", "Anchor Point Annotation Units", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0005, "CS", "Graphic Annotation Units", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0006, "ST", "Unformatted Text Value", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0008, "SQ", "Text Object Sequence", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0009, "SQ", "Graphic Object Sequence", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0010, "FL", "Bounding Box TLHC", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0011, "FL", "Bounding Box BRHC", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0014, "FL", "Anchor Point", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0015, "CS", "Anchor Point Visibility", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0020, "US", "Graphic Dimensions", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0021, "US", "Number Of Graphic Points", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0022, "FL", "Graphic Data", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0023, "CS", "Graphic Type", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0024, "CS", "Graphic Filled", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0040, "IS", "Image Rotation", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0041, "CS", "Image Horizontal Flip", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0050, "US", "Displayed Area TLHC", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0051, "US", "Displayed Area BRHC", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0060, "SQ", "Graphic Layer Sequence", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0062, "IS", "Graphic Layer Order", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0066, "US", "Graphic Layer Recommended Display Value", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0068, "LO", "Graphic Layer Description", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0080, "CS", "Presentation Label", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0081, "LO", "Presentation Description", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0082, "DA", "Presentation Creation Date", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0083, "TM", "Presentation Creation Time", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0084, "PN", "Presentation Creator's Name", (DicomElemParseFunc *) NULL },
    { 0x0087, 0x0010, "CS", "Media Type", (DicomElemParseFunc *) NULL },
    { 0x0087, 0x0020, "CS", "Media Location", (DicomElemParseFunc *) NULL },
    { 0x0087, 0x0050, "IS", "Estimated Retrieve Time", (DicomElemParseFunc *) NULL },
    { 0x0088, 0x0000, "UL", "Storage Group Length", (DicomElemParseFunc *) NULL },
    { 0x0088, 0x0130, "SH", "Storage Media FileSet ID", (DicomElemParseFunc *) NULL },
    { 0x0088, 0x0140, "UI", "Storage Media FileSet UID", (DicomElemParseFunc *) NULL },
    { 0x0088, 0x0200, "SQ", "Icon Image Sequence", (DicomElemParseFunc *) NULL },
    { 0x0088, 0x0904, "LO", "Topic Title", (DicomElemParseFunc *) NULL },
    { 0x0088, 0x0906, "ST", "Topic Subject", (DicomElemParseFunc *) NULL },
    { 0x0088, 0x0910, "LO", "Topic Author", (DicomElemParseFunc *) NULL },
    { 0x0088, 0x0912, "LO", "Topic Key Words", (DicomElemParseFunc *) NULL },
    { 0x0095, 0x0001, "LT", "Examination Folder ID", (DicomElemParseFunc *) NULL },
    { 0x0095, 0x0004, "UL", "Folder Reported Status", (DicomElemParseFunc *) NULL },
    { 0x0095, 0x0005, "LT", "Folder Reporting Radiologist", (DicomElemParseFunc *) NULL },
    { 0x0095, 0x0007, "LT", "SIENET ISA PLA", (DicomElemParseFunc *) NULL },
    { 0x0099, 0x0002, "UL", "Data Object Attributes", (DicomElemParseFunc *) NULL },
    { 0x00e1, 0x0001, "US", "Data Dictionary Version", (DicomElemParseFunc *) NULL },
    { 0x00e1, 0x0014, "LT", "?", (DicomElemParseFunc *) NULL },
    { 0x00e1, 0x0022, "DS", "?", (DicomElemParseFunc *) NULL },
    { 0x00e1, 0x0023, "DS", "?", (DicomElemParseFunc *) NULL },
    { 0x00e1, 0x0024, "LT", "?", (DicomElemParseFunc *) NULL },
    { 0x00e1, 0x0025, "LT", "?", (DicomElemParseFunc *) NULL },
    { 0x00e1, 0x0040, "SH", "Offset From CT MR Images", (DicomElemParseFunc *) NULL },
    { 0x0193, 0x0002, "DS", "RIS Key", (DicomElemParseFunc *) NULL },
    { 0x0307, 0x0001, "UN", "RIS Worklist IMGEF", (DicomElemParseFunc *) NULL },
    { 0x0309, 0x0001, "UN", "RIS Report IMGEF", (DicomElemParseFunc *) NULL },
    { 0x0601, 0x0000, "SH", "Implementation Version", (DicomElemParseFunc *) NULL },
    { 0x0601, 0x0020, "DS", "Relative Table Position", (DicomElemParseFunc *) NULL },
    { 0x0601, 0x0021, "DS", "Relative Table Height", (DicomElemParseFunc *) NULL },
    { 0x0601, 0x0030, "SH", "Surview Direction", (DicomElemParseFunc *) NULL },
    { 0x0601, 0x0031, "DS", "Surview Length", (DicomElemParseFunc *) NULL },
    { 0x0601, 0x0050, "SH", "Image View Type", (DicomElemParseFunc *) NULL },
    { 0x0601, 0x0070, "DS", "Batch Number", (DicomElemParseFunc *) NULL },
    { 0x0601, 0x0071, "DS", "Batch Size", (DicomElemParseFunc *) NULL },
    { 0x0601, 0x0072, "DS", "Batch Slice Number", (DicomElemParseFunc *) NULL },
    { 0x1000, 0x0000, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x1000, 0x0001, "US", "Run Length Triplet", (DicomElemParseFunc *) NULL },
    { 0x1000, 0x0002, "US", "Huffman Table Size", (DicomElemParseFunc *) NULL },
    { 0x1000, 0x0003, "US", "Huffman Table Triplet", (DicomElemParseFunc *) NULL },
    { 0x1000, 0x0004, "US", "Shift Table Size", (DicomElemParseFunc *) NULL },
    { 0x1000, 0x0005, "US", "Shift Table Triplet", (DicomElemParseFunc *) NULL },
    { 0x1010, 0x0000, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x1369, 0x0000, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x2000, 0x0000, "UL", "Film Session Group Length", (DicomElemParseFunc *) NULL },
    { 0x2000, 0x0010, "IS", "Number of Copies", (DicomElemParseFunc *) NULL },
    { 0x2000, 0x0020, "CS", "Print Priority", (DicomElemParseFunc *) NULL },
    { 0x2000, 0x0030, "CS", "Medium Type", (DicomElemParseFunc *) NULL },
    { 0x2000, 0x0040, "CS", "Film Destination", (DicomElemParseFunc *) NULL },
    { 0x2000, 0x0050, "LO", "Film Session Label", (DicomElemParseFunc *) NULL },
    { 0x2000, 0x0060, "IS", "Memory Allocation", (DicomElemParseFunc *) NULL },
    { 0x2000, 0x0500, "SQ", "Referenced Film Box Sequence", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0000, "UL", "Film Box Group Length", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0010, "ST", "Image Display Format", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0030, "CS", "Annotation Display Format ID", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0040, "CS", "Film Orientation", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0050, "CS", "Film Size ID", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0060, "CS", "Magnification Type", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0080, "CS", "Smoothing Type", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0100, "CS", "Border Density", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0110, "CS", "Empty Image Density", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0120, "US", "Min Density", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0130, "US", "Max Density", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0140, "CS", "Trim", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0150, "ST", "Configuration Information", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0500, "SQ", "Referenced Film Session Sequence", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0510, "SQ", "Referenced Image Box Sequence", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0520, "SQ", "Referenced Basic Annotation Box Sequence", (DicomElemParseFunc *) NULL },
    { 0x2020, 0x0000, "UL", "Image Box Group Length", (DicomElemParseFunc *) NULL },
    { 0x2020, 0x0010, "US", "Image Box Position", (DicomElemParseFunc *) NULL },
    { 0x2020, 0x0020, "CS", "Polarity", (DicomElemParseFunc *) NULL },
    { 0x2020, 0x0030, "DS", "Requested Image Size", (DicomElemParseFunc *) NULL },
    { 0x2020, 0x0110, "SQ", "Preformatted Grayscale Image Sequence", (DicomElemParseFunc *) NULL },
    { 0x2020, 0x0111, "SQ", "Preformatted Color Image Sequence", (DicomElemParseFunc *) NULL },
    { 0x2020, 0x0130, "SQ", "Referenced Image Overlay Box Sequence", (DicomElemParseFunc *) NULL },
    { 0x2020, 0x0140, "SQ", "Referenced VOI LUT Box Sequence", (DicomElemParseFunc *) NULL },
    { 0x2030, 0x0000, "UL", "Annotation Group Length", (DicomElemParseFunc *) NULL },
    { 0x2030, 0x0010, "US", "Annotation Position", (DicomElemParseFunc *) NULL },
    { 0x2030, 0x0020, "LO", "Text String", (DicomElemParseFunc *) NULL },
    { 0x2040, 0x0000, "UL", "Overlay Box Group Length", (DicomElemParseFunc *) NULL },
    { 0x2040, 0x0010, "SQ", "Referenced Overlay Plane Sequence", (DicomElemParseFunc *) NULL },
    { 0x2040, 0x0011, "US", "Referenced Overlay Plane Groups", (DicomElemParseFunc *) NULL },
    { 0x2040, 0x0060, "CS", "Overlay Magnification Type", (DicomElemParseFunc *) NULL },
    { 0x2040, 0x0070, "CS", "Overlay Smoothing Type", (DicomElemParseFunc *) NULL },
    { 0x2040, 0x0080, "CS", "Overlay Foreground Density", (DicomElemParseFunc *) NULL },
    { 0x2040, 0x0090, "CS", "Overlay Mode", (DicomElemParseFunc *) NULL },
    { 0x2040, 0x0100, "CS", "Threshold Density", (DicomElemParseFunc *) NULL },
    { 0x2040, 0x0500, "SQ", "Referenced Overlay Image Box Sequence", (DicomElemParseFunc *) NULL },
    { 0x2050, 0x0010, "SQ", "Presentation LUT Sequence", (DicomElemParseFunc *) NULL },
    { 0x2050, 0x0020, "CS", "Presentation LUT Shape", (DicomElemParseFunc *) NULL },
    { 0x2100, 0x0000, "UL", "Print Job Group Length", (DicomElemParseFunc *) NULL },
    { 0x2100, 0x0020, "CS", "Execution Status", (DicomElemParseFunc *) NULL },
    { 0x2100, 0x0030, "CS", "Execution Status Info", (DicomElemParseFunc *) NULL },
    { 0x2100, 0x0040, "DA", "Creation Date", (DicomElemParseFunc *) NULL },
    { 0x2100, 0x0050, "TM", "Creation Time", (DicomElemParseFunc *) NULL },
    { 0x2100, 0x0070, "AE", "Originator", (DicomElemParseFunc *) NULL },
    { 0x2100, 0x0500, "SQ", "Referenced Print Job Sequence", (DicomElemParseFunc *) NULL },
    { 0x2110, 0x0000, "UL", "Printer Group Length", (DicomElemParseFunc *) NULL },
    { 0x2110, 0x0010, "CS", "Printer Status", (DicomElemParseFunc *) NULL },
    { 0x2110, 0x0020, "CS", "Printer Status Info", (DicomElemParseFunc *) NULL },
    { 0x2110, 0x0030, "LO", "Printer Name", (DicomElemParseFunc *) NULL },
    { 0x2110, 0x0099, "SH", "Print Queue ID", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0002, "SH", "RT Image Label", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0003, "LO", "RT Image Name", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0004, "ST", "RT Image Description", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x000a, "CS", "Reported Values Origin", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x000c, "CS", "RT Image Plane", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x000e, "DS", "X-Ray Image Receptor Angle", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0010, "DS", "RTImageOrientation", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0011, "DS", "Image Plane Pixel Spacing", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0012, "DS", "RT Image Position", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0020, "SH", "Radiation Machine Name", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0022, "DS", "Radiation Machine SAD", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0024, "DS", "Radiation Machine SSD", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0026, "DS", "RT Image SID", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0028, "DS", "Source to Reference Object Distance", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0029, "IS", "Fraction Number", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0030, "SQ", "Exposure Sequence", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0032, "DS", "Meterset Exposure", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0001, "CS", "DVH Type", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0002, "CS", "Dose Units", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0004, "CS", "Dose Type", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0006, "LO", "Dose Comment", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0008, "DS", "Normalization Point", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x000a, "CS", "Dose Summation Type", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x000c, "DS", "GridFrame Offset Vector", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x000e, "DS", "Dose Grid Scaling", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0010, "SQ", "RT Dose ROI Sequence", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0012, "DS", "Dose Value", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0040, "DS", "DVH Normalization Point", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0042, "DS", "DVH Normalization Dose Value", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0050, "SQ", "DVH Sequence", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0052, "DS", "DVH Dose Scaling", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0054, "CS", "DVH Volume Units", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0056, "IS", "DVH Number of Bins", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0058, "DS", "DVH Data", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0060, "SQ", "DVH Referenced ROI Sequence", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0062, "CS", "DVH ROI Contribution Type", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0070, "DS", "DVH Minimum Dose", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0072, "DS", "DVH Maximum Dose", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0074, "DS", "DVH Mean Dose", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0002, "SH", "Structure Set Label", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0004, "LO", "Structure Set Name", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0006, "ST", "Structure Set Description", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0008, "DA", "Structure Set Date", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0009, "TM", "Structure Set Time", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0010, "SQ", "Referenced Frame of Reference Sequence", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0012, "SQ", "RT Referenced Study Sequence", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0014, "SQ", "RT Referenced Series Sequence", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0016, "SQ", "Contour Image Sequence", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0020, "SQ", "Structure Set ROI Sequence", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0022, "IS", "ROI Number", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0024, "UI", "Referenced Frame of Reference UID", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0026, "LO", "ROI Name", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0028, "ST", "ROI Description", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x002a, "IS", "ROI Display Color", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x002c, "DS", "ROI Volume", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0030, "SQ", "RT Related ROI Sequence", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0033, "CS", "RT ROI Relationship", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0036, "CS", "ROI Generation Algorithm", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0038, "LO", "ROI Generation Description", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0039, "SQ", "ROI Contour Sequence", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0040, "SQ", "Contour Sequence", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0042, "CS", "Contour Geometric Type", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0044, "DS", "Contour SlabT hickness", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0045, "DS", "Contour Offset Vector", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0046, "IS", "Number of Contour Points", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0050, "DS", "Contour Data", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0080, "SQ", "RT ROI Observations Sequence", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0082, "IS", "Observation Number", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0084, "IS", "Referenced ROI Number", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0085, "SH", "ROI Observation Label", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0086, "SQ", "RT ROI Identification Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0088, "ST", "ROI Observation Description", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x00a0, "SQ", "Related RT ROI Observations Sequence", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x00a4, "CS", "RT ROI Interpreted Type", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x00a6, "PN", "ROI Interpreter", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x00b0, "SQ", "ROI Physical Properties Sequence", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x00b2, "CS", "ROI Physical Property", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x00b4, "DS", "ROI Physical Property Value", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x00c0, "SQ", "Frame of Reference Relationship Sequence", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x00c2, "UI", "Related Frame of Reference UID", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x00c4, "CS", "Frame of Reference Transformation Type", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x00c6, "DS", "Frame of Reference Transformation Matrix", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x00c8, "LO", "Frame of Reference Transformation Comment", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0002, "SH", "RT Plan Label", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0003, "LO", "RT Plan Name", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0004, "ST", "RT Plan Description", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0006, "DA", "RT Plan Date", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0007, "TM", "RT Plan Time", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0009, "LO", "Treatment Protocols", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x000a, "CS", "Treatment Intent", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x000b, "LO", "Treatment Sites", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x000c, "CS", "RT Plan Geometry", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x000e, "ST", "Prescription Description", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0010, "SQ", "Dose ReferenceSequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0012, "IS", "Dose ReferenceNumber", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0014, "CS", "Dose Reference Structure Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0016, "LO", "Dose ReferenceDescription", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0018, "DS", "Dose Reference Point Coordinates", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x001a, "DS", "Nominal Prior Dose", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0020, "CS", "Dose Reference Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0021, "DS", "Constraint Weight", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0022, "DS", "Delivery Warning Dose", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0023, "DS", "Delivery Maximum Dose", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0025, "DS", "Target Minimum Dose", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0026, "DS", "Target Prescription Dose", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0027, "DS", "Target Maximum Dose", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0028, "DS", "Target Underdose Volume Fraction", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x002a, "DS", "Organ at Risk Full-volume Dose", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x002b, "DS", "Organ at Risk Limit Dose", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x002c, "DS", "Organ at Risk Maximum Dose", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x002d, "DS", "Organ at Risk Overdose Volume Fraction", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0040, "SQ", "Tolerance Table Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0042, "IS", "Tolerance Table Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0043, "SH", "Tolerance Table Label", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0044, "DS", "Gantry Angle Tolerance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0046, "DS", "Beam Limiting Device Angle Tolerance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0048, "SQ", "Beam Limiting Device Tolerance Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x004a, "DS", "Beam Limiting Device Position Tolerance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x004c, "DS", "Patient Support Angle Tolerance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x004e, "DS", "Table Top Eccentric Angle Tolerance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0051, "DS", "Table Top Vertical Position Tolerance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0052, "DS", "Table Top Longitudinal Position Tolerance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0053, "DS", "Table Top Lateral Position Tolerance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0055, "CS", "RT Plan Relationship", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0070, "SQ", "Fraction Group Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0071, "IS", "Fraction Group Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0078, "IS", "Number of Fractions Planned", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0079, "IS", "Number of Fractions Per Day", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x007a, "IS", "Repeat Fraction Cycle Length", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x007b, "LT", "Fraction Pattern", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0080, "IS", "Number of Beams", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0082, "DS", "Beam Dose Specification Point", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0084, "DS", "Beam Dose", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0086, "DS", "Beam Meterset", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00a0, "IS", "Number of Brachy Application Setups", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00a2, "DS", "Brachy Application Setup Dose Specification Point", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00a4, "DS", "Brachy Application Setup Dose", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00b0, "SQ", "Beam Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00b2, "SH", "Treatment Machine Name ", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00b3, "CS", "Primary Dosimeter Unit", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00b4, "DS", "Source-Axis Distance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00b6, "SQ", "Beam Limiting Device Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00b8, "CS", "RT Beam Limiting Device Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00ba, "DS", "Source to Beam Limiting Device Distance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00bc, "IS", "Number of Leaf/Jaw Pairs", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00be, "DS", "Leaf Position Boundaries", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00c0, "IS", "Beam Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00c2, "LO", "Beam Name", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00c3, "ST", "Beam Description", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00c4, "CS", "Beam Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00c6, "CS", "Radiation Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00c8, "IS", "Reference Image Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00ca, "SQ", "Planned Verification Image Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00cc, "LO", "Imaging Device Specific Acquisition Parameters", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00ce, "CS", "Treatment Delivery Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00d0, "IS", "Number of Wedges", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00d1, "SQ", "Wedge Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00d2, "IS", "Wedge Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00d3, "CS", "Wedge Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00d4, "SH", "Wedge ID", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00d5, "IS", "Wedge Angle", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00d6, "DS", "Wedge Factor", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00d8, "DS", "Wedge Orientation", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00da, "DS", "Source to Wedge Tray Distance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00e0, "IS", "Number of Compensators", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00e1, "SH", "Material ID", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00e2, "DS", "Total Compensator Tray Factor", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00e3, "SQ", "Compensator Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00e4, "IS", "Compensator Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00e5, "SH", "Compensator ID", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00e6, "DS", "Source to Compensator Tray Distance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00e7, "IS", "Compensator Rows", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00e8, "IS", "Compensator Columns", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00e9, "DS", "Compensator Pixel Spacing", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00ea, "DS", "Compensator Position", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00eb, "DS", "Compensator Transmission Data", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00ec, "DS", "Compensator Thickness Data", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00ed, "IS", "Number of Boli", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00f0, "IS", "Number of Blocks", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00f2, "DS", "Total Block Tray Factor", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00f4, "SQ", "Block Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00f5, "SH", "Block Tray ID", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00f6, "DS", "Source to Block Tray Distance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00f8, "CS", "Block Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00fa, "CS", "Block Divergence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00fc, "IS", "Block Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00fe, "LO", "Block Name", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0100, "DS", "Block Thickness", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0102, "DS", "Block Transmission", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0104, "IS", "Block Number of Points", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0106, "DS", "Block Data", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0107, "SQ", "Applicator Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0108, "SH", "Applicator ID", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0109, "CS", "Applicator Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x010a, "LO", "Applicator Description", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x010c, "DS", "Cumulative Dose Reference Coefficient", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x010e, "DS", "Final Cumulative Meterset Weight", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0110, "IS", "Number of Control Points", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0111, "SQ", "Control Point Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0112, "IS", "Control Point Index", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0114, "DS", "Nominal Beam Energy", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0115, "DS", "Dose Rate Set", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0116, "SQ", "Wedge Position Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0118, "CS", "Wedge Position", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x011a, "SQ", "Beam Limiting Device Position Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x011c, "DS", "Leaf Jaw Positions", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x011e, "DS", "Gantry Angle", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x011f, "CS", "Gantry Rotation Direction", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0120, "DS", "Beam Limiting Device Angle", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0121, "CS", "Beam Limiting Device Rotation Direction", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0122, "DS", "Patient Support Angle", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0123, "CS", "Patient Support Rotation Direction", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0124, "DS", "Table Top Eccentric Axis Distance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0125, "DS", "Table Top Eccentric Angle", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0126, "CS", "Table Top Eccentric Rotation Direction", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0128, "DS", "Table Top Vertical Position", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0129, "DS", "Table Top Longitudinal Position", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x012a, "DS", "Table Top Lateral Position", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x012c, "DS", "Isocenter Position", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x012e, "DS", "Surface Entry Point", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0130, "DS", "Source to Surface Distance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0134, "DS", "Cumulative Meterset Weight", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0180, "SQ", "Patient Setup Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0182, "IS", "Patient Setup Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0184, "LO", "Patient Additional Position", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0190, "SQ", "Fixation Device Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0192, "CS", "Fixation Device Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0194, "SH", "Fixation Device Label", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0196, "ST", "Fixation Device Description", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0198, "SH", "Fixation Device Position", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01a0, "SQ", "Shielding Device Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01a2, "CS", "Shielding Device Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01a4, "SH", "Shielding Device Label", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01a6, "ST", "Shielding Device Description", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01a8, "SH", "Shielding Device Position", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01b0, "CS", "Setup Technique", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01b2, "ST", "Setup TechniqueDescription", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01b4, "SQ", "Setup Device Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01b6, "CS", "Setup Device Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01b8, "SH", "Setup Device Label", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01ba, "ST", "Setup Device Description", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01bc, "DS", "Setup Device Parameter", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01d0, "ST", "Setup ReferenceDescription", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01d2, "DS", "Table Top Vertical Setup Displacement", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01d4, "DS", "Table Top Longitudinal Setup Displacement", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01d6, "DS", "Table Top Lateral Setup Displacement", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0200, "CS", "Brachy Treatment Technique", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0202, "CS", "Brachy Treatment Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0206, "SQ", "Treatment Machine Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0210, "SQ", "Source Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0212, "IS", "Source Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0214, "CS", "Source Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0216, "LO", "Source Manufacturer", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0218, "DS", "Active Source Diameter", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x021a, "DS", "Active Source Length", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0222, "DS", "Source Encapsulation Nominal Thickness", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0224, "DS", "Source Encapsulation Nominal Transmission", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0226, "LO", "Source IsotopeName", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0228, "DS", "Source Isotope Half Life", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x022a, "DS", "Reference Air Kerma Rate", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x022c, "DA", "Air Kerma Rate Reference Date", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x022e, "TM", "Air Kerma Rate Reference Time", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0230, "SQ", "Application Setup Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0232, "CS", "Application Setup Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0234, "IS", "Application Setup Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0236, "LO", "Application Setup Name", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0238, "LO", "Application Setup Manufacturer", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0240, "IS", "Template Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0242, "SH", "Template Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0244, "LO", "Template Name", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0250, "DS", "Total Reference Air Kerma", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0260, "SQ", "Brachy Accessory Device Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0262, "IS", "Brachy Accessory Device Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0263, "SH", "Brachy Accessory Device ID", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0264, "CS", "Brachy Accessory Device Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0266, "LO", "Brachy Accessory Device Name", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x026a, "DS", "Brachy Accessory Device Nominal Thickness", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x026c, "DS", "Brachy Accessory Device Nominal Transmission", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0280, "SQ", "Channel Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0282, "IS", "Channel Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0284, "DS", "Channel Length", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0286, "DS", "Channel Total Time", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0288, "CS", "Source Movement Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x028a, "IS", "Number of Pulses", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x028c, "DS", "Pulse Repetition Interval", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0290, "IS", "Source Applicator Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0291, "SH", "Source Applicator ID", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0292, "CS", "Source Applicator Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0294, "LO", "Source Applicator Name", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0296, "DS", "Source Applicator Length", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0298, "LO", "Source Applicator Manufacturer", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x029c, "DS", "Source Applicator Wall Nominal Thickness", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x029e, "DS", "Source Applicator Wall Nominal Transmission", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02a0, "DS", "Source Applicator Step Size", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02a2, "IS", "Transfer Tube Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02a4, "DS", "Transfer Tube Length", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02b0, "SQ", "Channel Shield Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02b2, "IS", "Channel Shield Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02b3, "SH", "Channel Shield ID", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02b4, "LO", "Channel Shield Name", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02b8, "DS", "Channel Shield Nominal Thickness", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02ba, "DS", "Channel Shield Nominal Transmission", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02c8, "DS", "Final Cumulative Time Weight", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02d0, "SQ", "Brachy Control Point Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02d2, "DS", "Control Point Relative Position", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02d4, "DS", "Control Point 3D Position", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02d6, "DS", "Cumulative Time Weight", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0002, "SQ", "Referenced RT Plan Sequence", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0004, "SQ", "Referenced Beam Sequence", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0006, "IS", "Referenced Beam Number", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0007, "IS", "Referenced Reference Image Number", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0008, "DS", "Start Cumulative Meterset Weight", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0009, "DS", "End Cumulative Meterset Weight", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x000a, "SQ", "Referenced Brachy Application Setup Sequence", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x000c, "IS", "Referenced Brachy Application Setup Number", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x000e, "IS", "Referenced Source Number", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0020, "SQ", "Referenced Fraction Group Sequence", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0022, "IS", "Referenced Fraction Group Number", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0040, "SQ", "Referenced Verification Image Sequence", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0042, "SQ", "Referenced Reference Image Sequence", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0050, "SQ", "Referenced Dose Reference Sequence", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0051, "IS", "Referenced Dose Reference Number", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0055, "SQ", "Brachy Referenced Dose Reference Sequence", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0060, "SQ", "Referenced Structure Set Sequence", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x006a, "IS", "Referenced Patient Setup Number", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0080, "SQ", "Referenced Dose Sequence", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x00a0, "IS", "Referenced Tolerance Table Number", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x00b0, "SQ", "Referenced Bolus Sequence", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x00c0, "IS", "Referenced Wedge Number", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x00d0, "IS", "Referenced Compensato rNumber", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x00e0, "IS", "Referenced Block Number", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x00f0, "IS", "Referenced Control Point", (DicomElemParseFunc *) NULL },
    { 0x300e, 0x0002, "CS", "Approval Status", (DicomElemParseFunc *) NULL },
    { 0x300e, 0x0004, "DA", "Review Date", (DicomElemParseFunc *) NULL },
    { 0x300e, 0x0005, "TM", "Review Time", (DicomElemParseFunc *) NULL },
    { 0x300e, 0x0008, "PN", "Reviewer Name", (DicomElemParseFunc *) NULL },
    { 0x4000, 0x0000, "UL", "Text Group Length", (DicomElemParseFunc *) NULL },
    { 0x4000, 0x0010, "LT", "Text Arbitrary", (DicomElemParseFunc *) NULL },
    { 0x4000, 0x4000, "LT", "Text Comments", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0000, "UL", "Results Group Length", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0040, "SH", "Results ID", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0042, "LO", "Results ID Issuer", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0050, "SQ", "Referenced Interpretation Sequence", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x00ff, "CS", "Report Production Status", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0100, "DA", "Interpretation Recorded Date", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0101, "TM", "Interpretation Recorded Time", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0102, "PN", "Interpretation Recorder", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0103, "LO", "Reference to Recorded Sound", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0108, "DA", "Interpretation Transcription Date", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0109, "TM", "Interpretation Transcription Time", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x010a, "PN", "Interpretation Transcriber", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x010b, "ST", "Interpretation Text", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x010c, "PN", "Interpretation Author", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0111, "SQ", "Interpretation Approver Sequence", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0112, "DA", "Interpretation Approval Date", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0113, "TM", "Interpretation Approval Time", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0114, "PN", "Physician Approving Interpretation", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0115, "LT", "Interpretation Diagnosis Description", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0117, "SQ", "InterpretationDiagnosis Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0118, "SQ", "Results Distribution List Sequence", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0119, "PN", "Distribution Name", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x011a, "LO", "Distribution Address", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0200, "SH", "Interpretation ID", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0202, "LO", "Interpretation ID Issuer", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0210, "CS", "Interpretation Type ID", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0212, "CS", "Interpretation Status ID", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0300, "ST", "Impressions", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x4000, "ST", "Results Comments", (DicomElemParseFunc *) NULL },
    { 0x4009, 0x0001, "LT", "Report ID", (DicomElemParseFunc *) NULL },
    { 0x4009, 0x0020, "LT", "Report Status", (DicomElemParseFunc *) NULL },
    { 0x4009, 0x0030, "DA", "Report Creation Date", (DicomElemParseFunc *) NULL },
    { 0x4009, 0x0070, "LT", "Report Approving Physician", (DicomElemParseFunc *) NULL },
    { 0x4009, 0x00e0, "LT", "Report Text", (DicomElemParseFunc *) NULL },
    { 0x4009, 0x00e1, "LT", "Report Author", (DicomElemParseFunc *) NULL },
    { 0x4009, 0x00e3, "LT", "Reporting Radiologist", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0000, "UL", "Curve Group Length", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0005, "US", "Curve Dimensions", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0010, "US", "Number of Points", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0020, "CS", "Type of Data", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0022, "LO", "Curve Description", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0030, "SH", "Axis Units", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0040, "SH", "Axis Labels", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0103, "US", "Data Value Representation", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0104, "US", "Minimum Coordinate Value", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0105, "US", "Maximum Coordinate Value", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0106, "SH", "Curve Range", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0110, "US", "Curve Data Descriptor", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0112, "US", "Coordinate Start Value", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0114, "US", "Coordinate Step Value", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x1001, "CS", "Curve Activation Layer", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x2000, "US", "Audio Type", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x2002, "US", "Audio Sample Format", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x2004, "US", "Number of Channels", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x2006, "UL", "Number of Samples", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x2008, "UL", "Sample Rate", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x200a, "UL", "Total Time", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x200c, "xs", "Audio Sample Data", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x200e, "LT", "Audio Comments", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x2500, "LO", "Curve Label", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x2600, "SQ", "CurveReferenced Overlay Sequence", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x2610, "US", "CurveReferenced Overlay Group", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x3000, "OW", "Curve Data", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0000, "UL", "Overlay Group Length", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0001, "US", "Gray Palette Color Lookup Table Descriptor", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0002, "US", "Gray Palette Color Lookup Table Data", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0010, "US", "Overlay Rows", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0011, "US", "Overlay Columns", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0012, "US", "Overlay Planes", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0015, "IS", "Number of Frames in Overlay", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0022, "LO", "Overlay Description", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0040, "CS", "Overlay Type", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0045, "CS", "Overlay Subtype", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0050, "SS", "Overlay Origin", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0051, "US", "Image Frame Origin", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0052, "US", "Plane Origin", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0060, "LO", "Overlay Compression Code", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0061, "SH", "Overlay Compression Originator", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0062, "SH", "Overlay Compression Label", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0063, "SH", "Overlay Compression Description", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0066, "AT", "Overlay Compression Step Pointers", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0068, "US", "Overlay Repeat Interval", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0069, "US", "Overlay Bits Grouped", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0100, "US", "Overlay Bits Allocated", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0102, "US", "Overlay Bit Position", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0110, "LO", "Overlay Format", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0200, "xs", "Overlay Location", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0800, "LO", "Overlay Code Label", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0802, "US", "Overlay Number of Tables", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0803, "AT", "Overlay Code Table Location", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0804, "US", "Overlay Bits For Code Word", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x1001, "CS", "Overlay Activation Layer", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x1100, "US", "Overlay Descriptor - Gray", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x1101, "US", "Overlay Descriptor - Red", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x1102, "US", "Overlay Descriptor - Green", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x1103, "US", "Overlay Descriptor - Blue", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x1200, "US", "Overlays - Gray", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x1201, "US", "Overlays - Red", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x1202, "US", "Overlays - Green", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x1203, "US", "Overlays - Blue", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x1301, "IS", "ROI Area", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x1302, "DS", "ROI Mean", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x1303, "DS", "ROI Standard Deviation", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x1500, "LO", "Overlay Label", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x3000, "OW", "Overlay Data", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x4000, "LT", "Overlay Comments", (DicomElemParseFunc *) NULL },
    { 0x6001, 0x0000, "UN", "?", (DicomElemParseFunc *) NULL },
    { 0x6001, 0x0010, "LO", "?", (DicomElemParseFunc *) NULL },
    { 0x6001, 0x1010, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x6001, 0x1030, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x6021, 0x0000, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x6021, 0x0010, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x7001, 0x0010, "LT", "Dummy", (DicomElemParseFunc *) NULL },
    { 0x7003, 0x0010, "LT", "Info", (DicomElemParseFunc *) NULL },
    { 0x7005, 0x0010, "LT", "Dummy", (DicomElemParseFunc *) NULL },
    { 0x7000, 0x0004, "ST", "TextAnnotation", (DicomElemParseFunc *) NULL },
    { 0x7000, 0x0005, "IS", "Box", (DicomElemParseFunc *) NULL },
    { 0x7000, 0x0007, "IS", "ArrowEnd", (DicomElemParseFunc *) NULL },
    { 0x7fe0, 0x0000, "UL", "Pixel Data Group Length", (DicomElemParseFunc *) NULL },
    { 0x7fe0, 0x0010, "xs", "Pixel Data", (DicomElemParseFunc *) NULL },
    { 0x7fe0, 0x0020, "OW", "Coefficients SDVN", (DicomElemParseFunc *) NULL },
    { 0x7fe0, 0x0030, "OW", "Coefficients SDHN", (DicomElemParseFunc *) NULL },
    { 0x7fe0, 0x0040, "OW", "Coefficients SDDN", (DicomElemParseFunc *) NULL },
    { 0x7fe1, 0x0010, "xs", "Pixel Data", (DicomElemParseFunc *) NULL },
    { 0x7f00, 0x0000, "UL", "Variable Pixel Data Group Length", (DicomElemParseFunc *) NULL },
    { 0x7f00, 0x0010, "xs", "Variable Pixel Data", (DicomElemParseFunc *) NULL },
    { 0x7f00, 0x0011, "US", "Variable Next Data Group", (DicomElemParseFunc *) NULL },
    { 0x7f00, 0x0020, "OW", "Variable Coefficients SDVN", (DicomElemParseFunc *) NULL },
    { 0x7f00, 0x0030, "OW", "Variable Coefficients SDHN", (DicomElemParseFunc *) NULL },
    { 0x7f00, 0x0040, "OW", "Variable Coefficients SDDN", (DicomElemParseFunc *) NULL },
    { 0x7fe1, 0x0000, "OB", "Binary Data", (DicomElemParseFunc *) NULL },
    { 0x7fe3, 0x0000, "LT", "Image Graphics Format Code", (DicomElemParseFunc *) NULL },
    { 0x7fe3, 0x0010, "OB", "Image Graphics", (DicomElemParseFunc *) NULL },
    { 0x7fe3, 0x0020, "OB", "Image Graphics Dummy", (DicomElemParseFunc *) NULL },
    { 0x7ff1, 0x0001, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x7ff1, 0x0002, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x7ff1, 0x0003, "xs", "?", (DicomElemParseFunc *) NULL },
    { 0x7ff1, 0x0004, "IS", "?", (DicomElemParseFunc *) NULL },
    { 0x7ff1, 0x0005, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x7ff1, 0x0007, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x7ff1, 0x0008, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x7ff1, 0x0009, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x7ff1, 0x000a, "LT", "?", (DicomElemParseFunc *) NULL },
    { 0x7ff1, 0x000b, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x7ff1, 0x000c, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x7ff1, 0x000d, "US", "?", (DicomElemParseFunc *) NULL },
    { 0x7ff1, 0x0010, "US", "?", (DicomElemParseFunc *) NULL },
    { 0xfffc, 0xfffc, "OB", "Data Set Trailing Padding", (DicomElemParseFunc *) NULL },
    { 0xfffe, 0xe000, "!!", "Item", (DicomElemParseFunc *) NULL },
    { 0xfffe, 0xe00d, "!!", "Item Delimitation Item", (DicomElemParseFunc *) NULL },
    { 0xfffe, 0xe0dd, "!!", "Sequence Delimitation Item", (DicomElemParseFunc *) NULL },
    { 0xffff, 0xffff, "xs", "", (DicomElemParseFunc *) NULL }
  };

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s D C M                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsDCM returns True if the image format type, identified by the
%  magick string, is DCM.
%
%  The format of the ReadDCMImage method is:
%
%      unsigned int IsDCM(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsDCM returns True if the image format type is DCM.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static MagickPassFail IsDCM(const unsigned char *magick,const size_t length)
{
  if (length < 132)
    return(False);
  if (LocaleNCompare((char *) (magick+128),"DICM",4) == 0)
    return(True);
  return(False);
}

static MagickPassFail DCM_InitDCM(DicomStream *dcm,int verbose)
{
  dcm->columns=0;
  dcm->rows=0;
  dcm->samples_per_pixel=1;
  dcm->bits_allocated=8;
  dcm->significant_bits=0;
  dcm->high_bit=0;
  dcm->bytes_per_pixel=1;
  dcm->max_value_in=255;
  dcm->max_value_out=255;
  dcm->pixel_representation=0;
  dcm->transfer_syntax=DCM_TS_IMPL_LITTLE;
  dcm->interlace=0;
  dcm->msb_state=DCM_MSB_LITTLE;
  dcm->phot_interp=DCM_PI_MONOCHROME2;
  dcm->window_center=0;
  dcm->window_width=0;
  dcm->rescale_intercept=0;
  dcm->rescale_slope=1;
  dcm->number_scenes=1;
  dcm->data=NULL;
  dcm->upper_lim=0;
  dcm->lower_lim=0;
  dcm->rescale_map=NULL;
  dcm->rescale_type=DCM_RT_HOUNSFIELD;
  dcm->rescaling=DCM_RS_NONE;
  dcm->offset_ct=0;
  dcm->offset_arr=NULL;
  dcm->frag_bytes=0;
  dcm->rle_rep_ct=0;
#if defined(USE_GRAYMAP)
  dcm->graymap=(unsigned short *) NULL;
#endif
  dcm->funcReadShort=ReadBlobLSBShort;
  dcm->funcReadLong=ReadBlobLSBLong;
  dcm->explicit_file=False;
  dcm->verbose=verbose;

  return MagickPass;
}

/*
  Parse functions for DICOM elements
*/
static MagickPassFail funcDCM_TransferSyntax(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  char
    *p;

  int
    type,
    subtype;

  if (dcm->data == (unsigned char *) NULL)
    {
      ThrowException(exception,CorruptImageError,ImproperImageHeader,image->filename);
      return MagickFail;
    }

  p=(char *) dcm->data;
  if (strncmp(p,"1.2.840.10008.1.2",17) == 0)
    {
      if (*(p+17) == 0)
        {
          dcm->transfer_syntax = DCM_TS_IMPL_LITTLE;
          return MagickPass;
        }
      type=0;
      subtype=0;
      /* Subtype is not always provided, but insist on type */
      if (sscanf(p+17,".%d.%d",&type,&subtype) < 1)
        {
          ThrowException(exception,CorruptImageError,ImproperImageHeader,image->filename);
          return MagickFail;
        }
      switch (type)
        {
        case 1:
          dcm->transfer_syntax = DCM_TS_EXPL_LITTLE;
          break;
        case 2:
          dcm->transfer_syntax = DCM_TS_EXPL_BIG;
          dcm->msb_state=DCM_MSB_BIG_PENDING;
          break;
        case 4:
          if ((subtype >= 80) && (subtype <= 81))
            dcm->transfer_syntax = DCM_TS_JPEG_LS;
          else
          if ((subtype >= 90) && (subtype <= 93))
            dcm->transfer_syntax = DCM_TS_JPEG_2000;
          else
            dcm->transfer_syntax = DCM_TS_JPEG;
          break;
        case 5:
          dcm->transfer_syntax = DCM_TS_RLE;
          break;
        }
    }
  return MagickPass;
}

static MagickPassFail funcDCM_StudyDate(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(exception);

  (void) SetImageAttribute(image,"StudyDate",(char *) dcm->data);
  return MagickPass;
}

static MagickPassFail funcDCM_PatientName(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(exception);

  (void) SetImageAttribute(image,"PatientName",(char *) dcm->data);
  return MagickPass;
}

static MagickPassFail funcDCM_TriggerTime(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(exception);

  (void) SetImageAttribute(image,"TriggerTime",(char *) dcm->data);
  return MagickPass;  
}

static MagickPassFail funcDCM_FieldOfView(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(exception);

  (void) SetImageAttribute(image,"FieldOfView",(char *) dcm->data);
  return MagickPass;
}

static MagickPassFail funcDCM_SeriesNumber(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(exception);
  (void) SetImageAttribute(image,"SeriesNumber",(char *) dcm->data);
  return MagickPass;
}

static MagickPassFail funcDCM_ImagePosition(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(exception);
  (void) SetImageAttribute(image,"ImagePosition",(char *) dcm->data);
  return MagickPass;
}

static MagickPassFail funcDCM_ImageOrientation(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(exception);
  (void) SetImageAttribute(image,"ImageOrientation",(char *) dcm->data);
  return MagickPass; 
}

static MagickPassFail funcDCM_SliceLocation(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(exception);
  (void) SetImageAttribute(image,"SliceLocation",(char *) dcm->data);
  return MagickPass;
}

static MagickPassFail funcDCM_SamplesPerPixel(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);
  dcm->samples_per_pixel=dcm->datum;
  return MagickPass;
}

static MagickPassFail funcDCM_PhotometricInterpretation(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  char photometric[MaxTextExtent];
  int i;

  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  if (dcm->data == (unsigned char *) NULL)
    {
      ThrowException(exception,CorruptImageError,ImproperImageHeader,image->filename);
      return MagickFail;
    }

  for (i=0; i < (long) Min(dcm->length, MaxTextExtent-1); i++)
    photometric[i]=dcm->data[i];
  photometric[i]='\0';

  if (strncmp(photometric,"MONOCHROME1",11) == 0)
    dcm->phot_interp = DCM_PI_MONOCHROME1;
  else
    if (strncmp(photometric,"MONOCHROME2",11) == 0)
      dcm->phot_interp = DCM_PI_MONOCHROME2;
    else
      if (strncmp(photometric,"PALETTE COLOR",13) == 0)
        dcm->phot_interp = DCM_PI_PALETTE_COLOR;
      else
        if (strncmp(photometric,"RGB",3) == 0)
          dcm->phot_interp = DCM_PI_RGB;
        else
          dcm->phot_interp = DCM_PI_OTHER;
  return MagickPass;
}

static MagickPassFail funcDCM_PlanarConfiguration(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  dcm->interlace=dcm->datum;
  return MagickPass;
}

static MagickPassFail funcDCM_NumberOfFrames(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  if (dcm->data == (unsigned char *) NULL)
    {
      ThrowException(exception,CorruptImageError,ImproperImageHeader,image->filename);
      return MagickFail;
    }

  dcm->number_scenes=MagickAtoI((char *) dcm->data);
  return MagickPass;
}

static MagickPassFail funcDCM_Rows(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  dcm->rows=dcm->datum;
  return MagickPass;
}

static MagickPassFail funcDCM_Columns(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  dcm->columns=dcm->datum;
  return MagickPass;
}

static MagickPassFail funcDCM_BitsAllocated(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  dcm->bits_allocated=dcm->datum;
  dcm->bytes_per_pixel=1;
  if (dcm->datum > 8)
    dcm->bytes_per_pixel=2;
  return MagickPass;
}

static MagickPassFail funcDCM_BitsStored(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(exception);

  dcm->significant_bits=dcm->datum;
  dcm->bytes_per_pixel=1;
  if (dcm->significant_bits > 8)
    dcm->bytes_per_pixel=2;
  dcm->max_value_in=(1 << dcm->significant_bits)-1;
  dcm->max_value_out=dcm->max_value_in;
  image->depth=Min(dcm->significant_bits,QuantumDepth);
  return MagickPass;
}

static MagickPassFail funcDCM_HighBit(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  dcm->high_bit=dcm->datum;
  return MagickPass;
}

static MagickPassFail funcDCM_PixelRepresentation(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  dcm->pixel_representation=dcm->datum;
  return MagickPass;
}

static MagickPassFail funcDCM_WindowCenter(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  char
    *p;

  if (dcm->data == (unsigned char *) NULL)
    {
      ThrowException(exception,CorruptImageError,ImproperImageHeader,image->filename);
      return MagickFail;
    }

  p = strrchr((char *) dcm->data,'\\');
  if (p)
    p++;
  else
    p=(char *) dcm->data;
  dcm->window_center=MagickAtoF(p);
  return MagickPass;
}

static MagickPassFail funcDCM_WindowWidth(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  char
    *p;

  if (dcm->data == (unsigned char *) NULL)
    {
      ThrowException(exception,CorruptImageError,ImproperImageHeader,image->filename);
      return MagickFail;
    }

  p = strrchr((char *) dcm->data,'\\');
  if (p)
    p++;
  else
    p=(char *) dcm->data;
  dcm->window_width=MagickAtoF(p);
  return MagickPass;
}

static MagickPassFail funcDCM_RescaleIntercept(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  char
    *p;

  if (dcm->data == (unsigned char *) NULL)
    {
      ThrowException(exception,CorruptImageError,ImproperImageHeader,image->filename);
      return MagickFail;
    }

  p = strrchr((char *) dcm->data,'\\');
  if (p)
    p++;
  else
    p=(char *) dcm->data;
  dcm->rescale_intercept=MagickAtoF(p);
  return MagickPass;
}

static MagickPassFail funcDCM_RescaleSlope(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  char
    *p;

  if (dcm->data == (unsigned char *) NULL)
    {
      ThrowException(exception,CorruptImageError,ImproperImageHeader,image->filename);
      return MagickFail;
    }

  p = strrchr((char *) dcm->data,'\\');
  if (p)
    p++;
  else
    p=(char *) dcm->data;
  dcm->rescale_slope=MagickAtoF(p);
  return MagickPass;
}

static MagickPassFail funcDCM_RescaleType(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{

  if (dcm->data == (unsigned char *) NULL)
    {
      ThrowException(exception,CorruptImageError,ImproperImageHeader,image->filename);
      return MagickFail;
    }

  if (strncmp((char *) dcm->data,"OD",2) == 0)
    dcm->rescale_type=DCM_RT_OPTICAL_DENSITY;
  else if (strncmp((char *) dcm->data,"HU",2) == 0)
    dcm->rescale_type=DCM_RT_HOUNSFIELD;
  else if (strncmp((char *) dcm->data,"US",2) == 0)
    dcm->rescale_type=DCM_RT_UNSPECIFIED;
  else
    dcm->rescale_type=DCM_RT_UNKNOWN;
  return MagickPass;
}

static MagickPassFail funcDCM_PaletteDescriptor(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  /*
    Palette descriptor tables
    element = 1101/2/3 = for red/green/blue palette
    val 0 = # of entries in LUT (0 for 65535)
    val 1 = min pix value in palette (ie pix <= val1 -> palette[0])
    val 2 = # bits in LUT (8 or 16)
    NB Required by specification to be the same for each color

    #1 - check the same each time
    #2 - use scale_remap to map vals <= (val1) to first entry
    and vals >= (val1 + palette size) to last entry
    #3 - if (val2) == 8, use UINT8 values instead of UINT16
    #4 - Check for UINT8 values packed into low bits of UINT16
    entries as per spec
  */
  ARG_NOT_USED(image);
  ARG_NOT_USED(dcm);
  ARG_NOT_USED(exception);

  return MagickPass;
}

static MagickPassFail funcDCM_LUT(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
#if defined(USE_GRAYMAP)  
  /*
    1200 = grey, LUT data 3006 = LUT data
  */
  unsigned long
    colors;

  register unsigned long
    i;

  if (dcm->data == (unsigned char *) NULL)
    {
      ThrowException(exception,CorruptImageError,ImproperImageHeader,image->filename);
      return MagickFail;
    }

  colors=dcm->length/dcm->bytes_per_pixel;
  dcm->datum=(long) colors;
  dcm->graymap=MagickAllocateArray(unsigned short *,colors,sizeof(unsigned short));
  if (dcm->graymap == (unsigned short *) NULL)
    {
      ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,image->filename);
      return MagickFail;
    }
  for (i=0; i < (long) colors; i++)
    if (dcm->bytes_per_pixel == 1)
      dcm->graymap[i]=dcm->data[i];
    else
      dcm->graymap[i]=(unsigned short) ((short *) dcm->data)[i];
#else
  ARG_NOT_USED(image);
  ARG_NOT_USED(dcm);
  ARG_NOT_USED(exception);
#endif
  return MagickPass;
}

static MagickPassFail funcDCM_Palette(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  register long
    i;

  unsigned char
    *p;

  unsigned short
    index;

  if (dcm->data == (unsigned char *) NULL)
    {
      ThrowException(exception,CorruptImageError,ImproperImageHeader,image->filename);
      return MagickFail;
    }

  /*
    Initialize colormap (entries are always 16 bit)
    1201/2/3 = red/green/blue palette
  */
  if (image->colormap == (PixelPacket *) NULL)
    {
      /*
        Allocate color map first time in
      */
      if (!AllocateImageColormap(image,(const unsigned long) dcm->length))
        {
          ThrowException(exception,ResourceLimitError,UnableToCreateColormap,image->filename);
          return MagickFail;
        }
    }

  /*
    Check that palette size matches previous one(s)
  */
  if (dcm->length != image->colors)
    {
      ThrowException(exception,ResourceLimitError,UnableToCreateColormap,image->filename);
      return MagickFail;
    }

  p=dcm->data;
  for (i=0; i < (long) image->colors; i++)
    {
      if (dcm->msb_state == DCM_MSB_BIG)
        index=(*p << 8) | *(p+1);
      else
        index=*p | (*(p+1) << 8);
      if (dcm->element == 0x1201)
        image->colormap[i].red=ScaleShortToQuantum(index);
      else if (dcm->element == 0x1202)
        image->colormap[i].green=ScaleShortToQuantum(index);
      else
        image->colormap[i].blue=ScaleShortToQuantum(index);
      p+=2;
    }
  return MagickPass;
}

static magick_uint8_t DCM_RLE_ReadByte(Image *image, DicomStream *dcm)
{
  if (dcm->rle_rep_ct == 0)
    {
      int
        rep_ct,
        rep_char;

      /* We need to read the next command pair */     
      if (dcm->frag_bytes <= 2)
        dcm->frag_bytes = 0;
      else
        dcm->frag_bytes -= 2;

      rep_ct=ReadBlobByte(image);
      rep_char=ReadBlobByte(image);
      if (rep_ct == 128)
        {
          /* Illegal value */
          return 0;
        }
      else
      if (rep_ct < 128)
        {
          /* (rep_ct+1) literal bytes */
          dcm->rle_rep_ct=rep_ct;
          dcm->rle_rep_char=-1;
          return (magick_uint8_t)rep_char;
        }
      else
        {
          /* (257-rep_ct) repeated bytes */
          dcm->rle_rep_ct=256-rep_ct;
          dcm->rle_rep_char=rep_char;
          return (magick_uint8_t)rep_char;
        }
    }

  dcm->rle_rep_ct--;
  if (dcm->rle_rep_char >= 0)
    return dcm->rle_rep_char;

  if (dcm->frag_bytes > 0)
    dcm->frag_bytes--;
  return ReadBlobByte(image);
}

static magick_uint16_t DCM_RLE_ReadShort(Image *image, DicomStream *dcm)
{
  return (DCM_RLE_ReadByte(image,dcm) << 4) | DCM_RLE_ReadByte(image,dcm);
}

static MagickPassFail DCM_ReadElement(Image *image, DicomStream *dcm,ExceptionInfo *exception)
{
  unsigned int
    use_explicit;

  char
    explicit_vr[MaxTextExtent],
    implicit_vr[MaxTextExtent];

  register long
    i;

  /*
    Read group and element IDs
  */
  image->offset=(long) TellBlob(image);
  dcm->group=dcm->funcReadShort(image);
  if ((dcm->msb_state == DCM_MSB_BIG_PENDING) && (dcm->group != 2))
    {
      dcm->group = (dcm->group << 8) | (dcm->group >> 8);
      dcm->funcReadShort=ReadBlobMSBShort;
      dcm->funcReadLong=ReadBlobMSBLong;
      dcm->msb_state=DCM_MSB_BIG;
    }
  dcm->element=dcm->funcReadShort(image);
  dcm->data=(unsigned char *) NULL;
  dcm->quantum=0;
  if (EOFBlob(image))
    {
      ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
      return MagickFail;
    }
  /*
    Find corresponding VR for this group and element.
  */
  for (i=0; dicom_info[i].group < 0xffffU; i++)
    if ((dcm->group == dicom_info[i].group) &&
        (dcm->element == dicom_info[i].element))
      break;
  dcm->index=i;

  /*
    Check for "explicitness", but meta-file headers always explicit.
  */
  if (ReadBlob(image,2,(char *) explicit_vr) != 2)
    {
      ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
      return MagickFail;
    }
  explicit_vr[2]='\0';
  (void) strlcpy(implicit_vr,dicom_info[dcm->index].vr,MaxTextExtent);

#if defined(NEW_IMPLICIT_LOGIC)
  use_explicit=False;
  if (isupper((int) *explicit_vr) && isupper((int) *(explicit_vr+1)))
    {
      /* Explicit VR looks to be valid */
      if (strcmp(explicit_vr,implicit_vr) == 0)
        {
          /* Explicit VR matches implicit VR so assume that it is explicit */
          use_explicit=True;
        }
      else if ((dcm->group & 1) || strcmp(implicit_vr,"xs") == 0)
        {
          /*
            We *must* use explicit type under two conditions
            1) group is odd and therefore private
            2) element vr is set as "xs" ie is not of a fixed type
          */
          use_explicit=True;
          strcpy(implicit_vr,explicit_vr);
        }
    }
  if ((!use_explicit) || (strcmp(implicit_vr,"!!") == 0))
    {
      /* Use implicit logic */
      (void) SeekBlob(image,(ExtendedSignedIntegralType) -2,SEEK_CUR);
      dcm->quantum=4;
    }
  else
    {
      /* Use explicit logic */
      dcm->quantum=2;
      if ((strcmp(explicit_vr,"OB") == 0) ||
          (strcmp(explicit_vr,"OW") == 0) ||
          (strcmp(explicit_vr,"OF") == 0) ||
          (strcmp(explicit_vr,"SQ") == 0) ||
          (strcmp(explicit_vr,"UN") == 0) ||
          (strcmp(explicit_vr,"UT") == 0))
        {
          (void) dcm->funcReadShort(image);
          if (EOFBlob(image))
            {
              ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
              return MagickFail;
            }
          dcm->quantum=4;
        }
    }
#else
  if (!dcm->explicit_file && (dcm->group != 0x0002))
    dcm->explicit_file=isupper((int) *explicit_vr) && isupper((int) *(explicit_vr+1));
  use_explicit=((dcm->group == 0x0002) || dcm->explicit_file);
  if (use_explicit && (strcmp(implicit_vr,"xs") == 0))
    (void) strlcpy(implicit_vr,explicit_vr,MaxTextExtent);
  if (!use_explicit || (strcmp(implicit_vr,"!!") == 0))
    {
      (void) SeekBlob(image,(ExtendedSignedIntegralType) -2,SEEK_CUR);
      dcm->quantum=4;
    }
  else
    {
      /*
        Assume explicit type.
      */
      dcm->quantum=2;
      if ((strcmp(explicit_vr,"OB") == 0) ||
          (strcmp(explicit_vr,"UN") == 0) ||
          (strcmp(explicit_vr,"OW") == 0) || (strcmp(explicit_vr,"SQ") == 0))
        {
          (void) dcm->funcReadShort(image)
            if (EOFBlob(image))
              {
                ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
                return MagickFail;
              }
          dcm->quantum=4;
        }
    }
#endif

  dcm->datum=0;
  if (dcm->quantum == 4)
    {
      dcm->datum=(long) dcm->funcReadLong(image);
      if (EOFBlob(image))
        {
          ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
          return MagickFail;
        }
    }
  else if (dcm->quantum == 2)
    {
      dcm->datum=(long) dcm->funcReadShort(image);
      if (EOFBlob(image))
        {
          ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
          return MagickFail;
        }
    }
  dcm->quantum=0;
  dcm->length=1;
  if (dcm->datum != 0)
    {
      if ((strcmp(implicit_vr,"SS") == 0) ||
          (strcmp(implicit_vr,"US") == 0) ||
          (strcmp(implicit_vr,"OW") == 0))
        dcm->quantum=2;
      else if ((strcmp(implicit_vr,"UL") == 0) ||
              (strcmp(implicit_vr,"SL") == 0) ||
              (strcmp(implicit_vr,"FL") == 0) ||
              (strcmp(implicit_vr,"OF") == 0))
        dcm->quantum=4;
      else  if (strcmp(implicit_vr,"FD") == 0)
        dcm->quantum=8;
      else
        dcm->quantum=1;

      if (dcm->datum != -1)
        {
          dcm->length=(size_t) dcm->datum/dcm->quantum;
        }
      else
        {
          /*
            Sequence and item of undefined length.
          */
          dcm->quantum=0;
          dcm->length=0;
        }
    }
  /*
    Display Dicom info.
  */
  if (dcm->verbose)
    {
      if (!use_explicit)
        explicit_vr[0]='\0';
      (void) fprintf(stdout,"0x%04lX %4lu %.1024s-%.1024s (0x%04x,0x%04x)",
                     image->offset,(unsigned long) dcm->length,implicit_vr,explicit_vr,
                     dcm->group,dcm->element);
      if (dicom_info[dcm->index].description != (char *) NULL)
        (void) fprintf(stdout," %.1024s",dicom_info[dcm->index].description);
      (void) fprintf(stdout,": ");
    }
  if ((dcm->group == 0x7FE0) && (dcm->element == 0x0010))
    {
      if (dcm->verbose)
        (void) fprintf(stdout,"\n");
      return MagickPass;
    }
  /*
    Allocate array and read data into it
  */
  if ((dcm->length == 1) && (dcm->quantum == 1))
    {
      if ((dcm->datum=ReadBlobByte(image)) == EOF)
        {
          ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
          return MagickFail;
        }
    }
  else if ((dcm->length == 1) && (dcm->quantum == 2))
    {
      dcm->datum=dcm->funcReadShort(image);
      if (EOFBlob(image))
        {
          ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
          return MagickFail;
        }
    }
  else if ((dcm->length == 1) && (dcm->quantum == 4))
    {
      dcm->datum=(long) dcm->funcReadLong(image);
      if (EOFBlob(image))
        {
          ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
          return MagickFail;
        }
    }
  else if ((dcm->quantum != 0) && (dcm->length != 0))
    {
      size_t
        size;

      if (dcm->length > ((~0UL)/dcm->quantum))
        {
          ThrowException(exception,CorruptImageError,ImproperImageHeader,image->filename);
          return MagickFail;
        }
      dcm->data=MagickAllocateArray(unsigned char *,(dcm->length+1),dcm->quantum);
      if (dcm->data == (unsigned char *) NULL)
        {
          ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,image->filename);
          return MagickFail;
        }
      size=dcm->quantum*dcm->length;
      if (ReadBlob(image,size,(char *) dcm->data) != size)
        {
          ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
          return MagickFail;
        }
      dcm->data[size]=0;
    }

  if (dcm->verbose)
    {
      /*
        Display data
      */
      if (dcm->data == (unsigned char *) NULL)
        {
          (void) fprintf(stdout,"%d\n",dcm->datum);
        }
      else
        {
          for (i=0; i < (long) Max(dcm->length,4); i++)
            if (!isprint(dcm->data[i]))
              break;
          if ((i != (long) dcm->length) && (dcm->length <= 4))
            {
              long
                j,
                bin_datum;

              bin_datum=0;
              for (j=(long) dcm->length-1; j >= 0; j--)
                bin_datum=256*bin_datum+dcm->data[j];
              (void) fprintf(stdout,"%lu\n",bin_datum);
            }
          else
            {
              for (i=0; i < (long) dcm->length; i++)
                if (isprint(dcm->data[i]))
                  (void) fprintf(stdout,"%c",dcm->data[i]);
                else
                  (void) fprintf(stdout,"%c",'.');
              (void) fprintf(stdout,"\n");
            }
        }
    }
  return MagickPass;
}

static MagickPassFail DCM_SetupColormap(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  if ((image->previous) && (image->previous->colormap != (PixelPacket*)NULL))
    {
      size_t
        length;

      /*
        Clone colormap from previous image
      */
      image->storage_class=PseudoClass;
      image->colors=image->previous->colors;
      length=image->colors*sizeof(PixelPacket);
      image->colormap=MagickAllocateMemory(PixelPacket *,length);
      if (image->colormap == (PixelPacket *) NULL)
        {
          ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,image->filename);
          return MagickFail;
        }
      (void) memcpy(image->colormap,image->previous->colormap,length);
    }
  else
    {
      /*
        Create new colormap
      */
      if (AllocateImageColormap(image,dcm->max_value_out+1) == MagickFail)
        {
          ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,image->filename);
          return MagickFail;
        }
    }
  return MagickPass;
}

static MagickPassFail DCM_SetupRescaleMap(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  /*
    rescale_map maps input sample range -> output colormap range combining rescale
    and window transforms, palette scaling and palette inversion (for MONOCHROME1)
    as well as allowing for pixel_representation of 1 which causes input samples to
    be treated as signed
  */
  double
    win_center,
    win_width,
    Xr,
    Xw_min,
    Xw_max;

  unsigned long
    i;

  if (dcm->rescaling == DCM_RS_NONE)
    return MagickPass;

  if (dcm->rescale_map == (Quantum *) NULL)
    {
      dcm->rescale_map=MagickAllocateArray(Quantum *,dcm->max_value_in+1,sizeof(Quantum));
      if (dcm->rescale_map == NULL)
        {
          ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,image->filename);
          return MagickFail;
        }
    }

  if (dcm->window_width == 0)
    { /* zero window width */
      if (dcm->upper_lim > dcm->lower_lim)
        {
          /* Use known range within image */
          win_width=(dcm->upper_lim-dcm->lower_lim+1)*dcm->rescale_slope;
          win_center=((dcm->upper_lim+dcm->lower_lim)/2)*dcm->rescale_slope+dcm->rescale_intercept;
        }
      else
        {
          /* Use full sample range and hope for the best */
          win_width=(dcm->max_value_in+1)*dcm->rescale_slope;
          if (dcm->pixel_representation == 1)
            win_center=dcm->rescale_intercept;
          else
            win_center=win_width/2 + dcm->rescale_intercept;
        }
    }
  else
    {
      win_width=dcm->window_width;
      win_center=dcm->window_center;
    }
  Xw_min = win_center - 0.5 - ((win_width-1)/2);
  Xw_max = win_center - 0.5 + ((win_width-1)/2);
  for (i=0; i < (dcm->max_value_in+1); i++)
    {
      if ((dcm->pixel_representation == 1) && (i >= (1U << (dcm->significant_bits-1))))
        Xr = -((dcm->max_value_in+1-i) * dcm->rescale_slope) + dcm->rescale_intercept;
      else
        Xr = (i * dcm->rescale_slope) + dcm->rescale_intercept;
      if (Xr <= Xw_min)
        dcm->rescale_map[i]=0;
      else if (Xr >= Xw_max)
        dcm->rescale_map[i]=dcm->max_value_out;
      else
        dcm->rescale_map[i]=(Quantum)(((Xr-Xw_min)/(win_width-1))*dcm->max_value_out+0.5);
    }  
  if (dcm->phot_interp == DCM_PI_MONOCHROME1)
    for (i=0; i < (dcm->max_value_in+1); i++)
      dcm->rescale_map[i]=dcm->max_value_out-dcm->rescale_map[i];
  return MagickPass;
}

void DCM_SetRescaling(DicomStream *dcm,int avoid_scaling)
{
  /*
    If avoid_scaling is True then scaling will only be applied where necessary ie
    input bit depth exceeds quantum size.

    ### TODO : Should this throw an error rather than setting DCM_RS_PRE?
  */
  dcm->rescaling=DCM_RS_NONE;
  dcm->max_value_out=dcm->max_value_in;

  if (dcm->phot_interp == DCM_PI_PALETTE_COLOR)
    {
      if (dcm->max_value_in >= MaxColormapSize)
        {
          dcm->max_value_out=MaxColormapSize-1;
          dcm->rescaling=DCM_RS_PRE;
        }
      return;
    }

  if ((dcm->phot_interp == DCM_PI_MONOCHROME1) ||
      (dcm->phot_interp == DCM_PI_MONOCHROME2))
    {
      if ((dcm->transfer_syntax == DCM_TS_JPEG) ||
          (dcm->transfer_syntax == DCM_TS_JPEG_LS) ||
          (dcm->transfer_syntax == DCM_TS_JPEG_2000))
        {
          /* Encapsulated non-native grayscale images are post rescaled by default */
          if (!avoid_scaling)
            dcm->rescaling=DCM_RS_POST;
        }
#if defined(GRAYSCALE_USES_PALETTE)
      else if (dcm->max_value_in >= MaxColormapSize)
        {
          dcm->max_value_out=MaxColormapSize-1;
          dcm->rescaling=DCM_RS_PRE;
        }
#else
      else if (dcm->max_value_in > MaxRGB)
        {
          dcm->max_value_out=MaxRGB;
          dcm->rescaling=DCM_RS_PRE;
        }
#endif
      else if (!avoid_scaling)
        {
#if !defined(GRAYSCALE_USES_PALETTE)
          dcm->max_value_out=MaxRGB;
#endif
          dcm->rescaling=DCM_RS_POST;
        }
      return;
    }

  if (avoid_scaling || (dcm->max_value_in == MaxRGB))
    return;

  dcm->max_value_out=MaxRGB;
  dcm->rescaling=DCM_RS_PRE;
}

static MagickPassFail DCM_PostRescaleImage(Image *image,DicomStream *dcm,unsigned long ScanLimits,ExceptionInfo *exception)
{
  unsigned long
    y,
    x;

  register PixelPacket
    *q;

  if (ScanLimits)
    {
      /*
        Causes rescan for upper/lower limits - used for encapsulated images
      */
      unsigned int
        l;

      for (y=0; y < image->rows; y++)
        {
          q=GetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            return MagickFail;

          if (image->storage_class == PseudoClass)
            {
              register IndexPacket
                *indexes;

              indexes=AccessMutableIndexes(image);
              for (x=0; x < image->columns; x++)
                {
                  l=indexes[x];
                  if (dcm->pixel_representation == 1)
                    if (l > ((dcm->max_value_in >> 1)))
                      l = dcm->max_value_in-l+1;
                  if (l < (unsigned int) dcm->lower_lim)
                    dcm->lower_lim = l;
                  if (l > (unsigned int) dcm->upper_lim)
                    dcm->upper_lim = l;
                }
            }
          else
            {
              for (x=0; x < image->columns; x++)
                {
                  l=q->green;
                  if (dcm->pixel_representation == 1)
                    if (l > (dcm->max_value_in >> 1))
                      l = dcm->max_value_in-l+1;
                  if (l < (unsigned int) dcm->lower_lim)
                    dcm->lower_lim = l;
                  if (l > (unsigned int) dcm->upper_lim)
                    dcm->upper_lim = l;
                  q++;
                }
            }
        }

      if (image->storage_class == PseudoClass)
        {
          /* Handle compressed range by reallocating palette */
          if (!AllocateImageColormap(image,dcm->upper_lim+1))
            {
              ThrowException(exception,ResourceLimitError,UnableToCreateColormap,image->filename);
              return MagickFail;
            }
          return MagickPass;
        }
    }

  DCM_SetupRescaleMap(image,dcm,exception);
  for (y=0; y < image->rows; y++)
    {
      q=GetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        return MagickFail;

      if (image->storage_class == PseudoClass)
        {
          register IndexPacket
            *indexes;

          indexes=AccessMutableIndexes(image);
          for (x=0; x < image->columns; x++)
            indexes[x]=dcm->rescale_map[indexes[x]];
        }
      else
        {
          for (x=0; x < image->columns; x++)
            {
              q->red=dcm->rescale_map[q->red];
              q->green=dcm->rescale_map[q->green];
              q->blue=dcm->rescale_map[q->blue];
              q++;
            }
        }
      if (!SyncImagePixels(image))
        return MagickFail;
      /*
        if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
        if (!MagickMonitorFormatted(y,image->rows,exception,
        LoadImageText,image->filename))
        return MagickFail;
      */
    }
  return MagickPass;
}

static MagickPassFail DCM_ReadPaletteImage(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  long
    y;

  register long
    x;

  register PixelPacket
    *q;

  register IndexPacket
    *indexes;

  unsigned short
    index;

  unsigned char
    byte;

  byte=0;

  for (y=0; y < (long) image->rows; y++)
    {
      q=SetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        return MagickFail;
      indexes=AccessMutableIndexes(image);
      for (x=0; x < (long) image->columns; x++)
        {
          if (dcm->bytes_per_pixel == 1)
            {
              if (dcm->transfer_syntax == DCM_TS_RLE)
                index=DCM_RLE_ReadByte(image,dcm);
              else
                index=ReadBlobByte(image);
            }
          else
          if (dcm->bits_allocated != 12)
            {
              if (dcm->transfer_syntax == DCM_TS_RLE)
                index=DCM_RLE_ReadShort(image,dcm);
              else
                index=dcm->funcReadShort(image);
            }
          else
            {
              if (x & 0x01)
                {
                  /* ### TODO - Check whether logic needs altering if msb_state != DCM_MSB_LITTLE */
                  if (dcm->transfer_syntax == DCM_TS_RLE)
                    index=DCM_RLE_ReadByte(image,dcm);
                  else
                    index=ReadBlobByte(image);
                  index=(index << 4) | byte;
                }
              else
                {
                  if (dcm->transfer_syntax == DCM_TS_RLE)
                    index=DCM_RLE_ReadByte(image,dcm);
                  else
                    index=dcm->funcReadShort(image);
                  byte=index >> 12;
                  index&=0xfff;
                }
            }
          index&=dcm->max_value_in;
          if (dcm->rescaling == DCM_RS_PRE)
            {
              /*
                ### TODO - must read as Direct Class so look up
                red/green/blue values in palette table, processed via
                rescale_map
              */
            }
          else
            {
#if defined(USE_GRAYMAP)
              if (dcm->graymap != (unsigned short *) NULL)
                index=dcm->graymap[index];
#endif
              index=(IndexPacket) (index);
              VerifyColormapIndex(image,index);
              indexes[x]=index;
            }

          if (EOFBlob(image))
            {
              ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
              return MagickFail;
            }
        }
      if (!SyncImagePixels(image))
        return MagickFail;
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          if (!MagickMonitorFormatted(y,image->rows,exception,
                                      LoadImageText,image->filename,
                                      image->columns,image->rows))
            return MagickFail;
    }
  return MagickPass;
}

static MagickPassFail DCM_ReadGrayscaleImage(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  long
    y;

  register long
    x;

  register PixelPacket
    *q;

#if defined(GRAYSCALE_USES_PALETTE)
  register IndexPacket
    *indexes;
#endif

  unsigned short
    index;

  unsigned char
    byte;

  dcm->lower_lim = dcm->max_value_in;
  dcm->upper_lim = -(dcm->lower_lim);
  byte=0;
  for (y=0; y < (long) image->rows; y++)
    {
      q=SetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        return MagickFail;
#if defined(GRAYSCALE_USES_PALETTE)
      indexes=AccessMutableIndexes(image);
#endif
      for (x=0; x < (long) image->columns; x++)
        {
          if (dcm->bytes_per_pixel == 1)
            {
              if (dcm->transfer_syntax == DCM_TS_RLE)
                index=DCM_RLE_ReadByte(image,dcm);
              else
                index=ReadBlobByte(image);
            }
          else
          if (dcm->bits_allocated != 12)
            {
              if (dcm->transfer_syntax == DCM_TS_RLE)
                index=DCM_RLE_ReadShort(image,dcm);
              else
                index=dcm->funcReadShort(image);
            }
          else
            {
              if (x & 0x01)
                {
                  /* ### TODO - Check whether logic needs altering if msb_state != DCM_MSB_LITTLE */
                  if (dcm->transfer_syntax == DCM_TS_RLE)
                    index=DCM_RLE_ReadByte(image,dcm);
                  else
                    index=ReadBlobByte(image);
                  index=(index << 4) | byte;
                }
              else
                {
                  if (dcm->transfer_syntax == DCM_TS_RLE)
                    index=DCM_RLE_ReadShort(image,dcm);
                  else
                    index=dcm->funcReadShort(image);
                  byte=index >> 12;
                  index&=0xfff;
                }
            }
          index&=dcm->max_value_in;
          if (dcm->rescaling == DCM_RS_POST)
            {
              unsigned int
                l;

              l = index;
              if (dcm->pixel_representation == 1)
                if (l > (dcm->max_value_in >> 1))
                  l = dcm->max_value_in-l+1;
              if ((int) l < dcm->lower_lim)
                dcm->lower_lim = l;
              if ((int) l > dcm->upper_lim)
                dcm->upper_lim = l;
            }
#if defined(GRAYSCALE_USES_PALETTE)
          if (dcm->rescaling == DCM_RS_PRE)
            indexes[x]=dcm->rescale_map[index];
          else
            indexes[x]=index;
#else
          if (dcm->rescaling == DCM_RS_PRE)
            index=dcm->rescale_map[index];
          q->red=index;
          q->green=index;
          q->blue=index;
          q++;
#endif
          if (EOFBlob(image))
            {
              ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
              return MagickFail;
            }
        }
      if (!SyncImagePixels(image))
        return MagickFail;
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          if (!MagickMonitorFormatted(y,image->rows,exception,
                                      LoadImageText,image->filename,
                                      image->columns,image->rows))
            return MagickFail;
    }
  return MagickPass;
}

static MagickPassFail DCM_ReadPlanarRGBImage(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  unsigned long
    plane,
    y,
    x;

  register PixelPacket
    *q;

  for (plane=0; plane < dcm->samples_per_pixel; plane++)
    {
      for (y=0; y < image->rows; y++)
        {
          q=GetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            return MagickFail;

          for (x=0; x < image->columns; x++)
            {
              switch ((int) plane)
                {
                  case 0:
                    if (dcm->transfer_syntax == DCM_TS_RLE)
                      q->red=ScaleCharToQuantum(DCM_RLE_ReadByte(image,dcm));
                    else
                      q->red=ScaleCharToQuantum(ReadBlobByte(image));
                    break;
                  case 1:
                    if (dcm->transfer_syntax == DCM_TS_RLE)
                      q->green=ScaleCharToQuantum(DCM_RLE_ReadByte(image,dcm));
                    else
                      q->green=ScaleCharToQuantum(ReadBlobByte(image));
                    break;
                  case 2:
                    if (dcm->transfer_syntax == DCM_TS_RLE)
                      q->blue=ScaleCharToQuantum(DCM_RLE_ReadByte(image,dcm));
                    else
                      q->blue=ScaleCharToQuantum(ReadBlobByte(image));
                    break;
                  case 3:
                    if (dcm->transfer_syntax == DCM_TS_RLE)
                      q->opacity=ScaleCharToQuantum((Quantum)(MaxRGB-ScaleCharToQuantum(DCM_RLE_ReadByte(image,dcm))));
                    else
                      q->opacity=ScaleCharToQuantum((Quantum)(MaxRGB-ScaleCharToQuantum(ReadBlobByte(image))));
                    break;
                }
              if (EOFBlob(image))
                {
                  ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
                  return MagickFail;
                }
              q++;
            }
          if (!SyncImagePixels(image))
            return MagickFail;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitorFormatted(y,image->rows,exception,
                                          LoadImageText,image->filename,
                                          image->columns,image->rows))
                return MagickFail;
        }
    }
  return MagickPass;
}

static MagickPassFail DCM_ReadRGBImage(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  unsigned long
    y,
    x;

  register PixelPacket
    *q;

  Quantum
    blue,
    green,
    red;

  red=0;
  green=0;
  blue=0;

  for (y=0; y < image->rows; y++)
    {
      q=GetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        return MagickFail;

      for (x=0; x < image->columns; x++)
        {
          if (dcm->bytes_per_pixel == 1)
            {
              if (dcm->transfer_syntax == DCM_TS_RLE)
                {
                  red=DCM_RLE_ReadByte(image,dcm);
                  green=DCM_RLE_ReadByte(image,dcm);
                  blue=DCM_RLE_ReadByte(image,dcm);

                }
              else
                {
                  red=ReadBlobByte(image);
                  green=ReadBlobByte(image);
                  blue=ReadBlobByte(image);
                }
            }
          else
            {
              if (dcm->transfer_syntax == DCM_TS_RLE)
                {
                  red=DCM_RLE_ReadShort(image,dcm);
                  green=DCM_RLE_ReadShort(image,dcm);
                  blue=DCM_RLE_ReadShort(image,dcm);
                }
              else
                {
                  red=dcm->funcReadShort(image);
                  green=dcm->funcReadShort(image);
                  blue=dcm->funcReadShort(image);
                }
            }
          red&=dcm->max_value_in;
          green&=dcm->max_value_in;
          blue&=dcm->max_value_in;
          if (dcm->rescaling == DCM_RS_PRE)
            {
              red=dcm->rescale_map[red];
              green=dcm->rescale_map[green];
              blue=dcm->rescale_map[blue];
            }

          q->red=(Quantum) red;
          q->green=(Quantum) green;
          q->blue=(Quantum) blue;
          q++;
          if (EOFBlob(image))
            {
              ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
              return MagickFail;
            }
        }
      if (!SyncImagePixels(image))
        return MagickFail;
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          if (!MagickMonitorFormatted(y,image->rows,exception,
                                      LoadImageText,image->filename,
                                      image->columns,image->rows))
            return MagickFail;
    }
  return MagickPass;
}

static MagickPassFail DCM_ReadOffsetTable(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  magick_uint32_t
    base_offset,
    tag,
    length,
    i;

  tag=(dcm->funcReadShort(image) << 16) | dcm->funcReadShort(image);
  length=dcm->funcReadLong(image);
  if (tag != 0xFFFEE000)
    return MagickFail;

  dcm->offset_ct=length >> 2;
  if (dcm->offset_ct == 0)
    return MagickPass;

  if (dcm->offset_ct != dcm->number_scenes)
    {
      ThrowException(exception,CorruptImageError,ImproperImageHeader,image->filename);
      return MagickFail;
    }

  dcm->offset_arr=MagickAllocateArray(magick_uint32_t *,dcm->offset_ct,sizeof(magick_uint32_t));  
  if (dcm->offset_arr == (magick_uint32_t *) NULL)
    {
      ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,image->filename);
      return MagickFail;
    }

  for (i=0; i < dcm->offset_ct; i++)
  {
    dcm->offset_arr[i]=dcm->funcReadLong(image);
    if (EOFBlob(image))
      return MagickFail;
  }
  base_offset=(magick_uint32_t)TellBlob(image);
  for (i=0; i < dcm->offset_ct; i++)
    dcm->offset_arr[i]+=base_offset;

  /*
     Seek first fragment of first frame if necessary (although it shouldn't be...)
  */
  if (TellBlob(image) != dcm->offset_arr[0])
    SeekBlob(image,dcm->offset_arr[0],SEEK_SET);
  return MagickPass;
}

static MagickPassFail DCM_ReadNonNativeImages(Image **image,const ImageInfo *image_info,DicomStream *dcm,ExceptionInfo *exception)
{
  char
    filename[MaxTextExtent];

  FILE
    *file;

  int
    c;

  Image
    *image_list,
    *next_image;

  ImageInfo
    *clone_info;

  magick_uint32_t
    scene,
    tag,
    status,
    length,
    i;

  image_list=(Image *)NULL;

  /*
    Read offset table
  */
  if (DCM_ReadOffsetTable(*image,dcm,exception) == MagickFail)
    return MagickFail;

  status=MagickPass;
  for (scene=0; scene < dcm->number_scenes; scene++)
    {
      /*
        Use temporary file to hold extracted data stream
      */
      file=AcquireTemporaryFileStream(filename,BinaryFileIOMode);
      if (file == (FILE *) NULL)
        {
          ThrowException(exception,FileOpenError,UnableToCreateTemporaryFile,filename);
          return MagickFail;
        }

      status=MagickPass;
      do
        {
          /*
            Read fragment tag
          */
          tag=(dcm->funcReadShort(*image) << 16) | dcm->funcReadShort(*image);
          length=dcm->funcReadLong(*image);
          if (EOFBlob(*image))
            {
              status=MagickFail;
              break;
            }

          if (tag == 0xFFFEE0DD)
            {
              /* Sequence delimiter tag */
              break;
            }
          else
            if (tag != 0xFFFEE000)
              {
                status=MagickFail;
                break;
              }

          /*
            Copy this fragment to the temporary file
          */
          while (length > 0)
            {
              c=ReadBlobByte(*image);
              if (c == EOF)
                {
                  status=MagickFail;
                  break;
                }
              (void) fputc(c,file);
              length--;
            }

          if (dcm->offset_ct == 0)
            {
              /*
                Assume one fragment per frame so break loop unless we are in the last frame
              */
              if (scene < dcm->number_scenes-1)
                break;
            }
          else
            {
              /* Look for end of multi-fragment frames by checking for against offset table */
              length=TellBlob(*image);
              for (i=0; i < dcm->offset_ct; i++)
                if (length == dcm->offset_arr[i])
                  {
                    break;
                  }
            }
        } while (status == MagickPass);

      (void) fclose(file);
      if (status == MagickPass)
        {
          clone_info=CloneImageInfo(image_info);
          clone_info->blob=(void *) NULL;
          clone_info->length=0;
          if (dcm->transfer_syntax == DCM_TS_JPEG_2000)
            FormatString(clone_info->filename,"jp2:%.1024s",filename);
          else
            FormatString(clone_info->filename,"jpeg:%.1024s",filename);
          next_image=ReadImage(clone_info,exception);
          DestroyImageInfo(clone_info);
          if (next_image == (Image*)NULL)
            {
              status=MagickFail;
            }
          else
            if (dcm->rescaling == DCM_RS_POST)
              {
                /*
                  ### TODO: ???
                  We cannot guarantee integrity of input data since libjpeg may already have
                  downsampled 12- or 16-bit jpegs. Best guess at the moment is to recalculate
                  scaling using the image depth (unless avoid-scaling is in force)
                */
                /* Allow for libjpeg having changed depth of image */
                dcm->significant_bits=next_image->depth;
                dcm->bytes_per_pixel=1;
                if (dcm->significant_bits > 8)
                  dcm->bytes_per_pixel=2;
                dcm->max_value_in=(1 << dcm->significant_bits)-1;
                dcm->max_value_out=dcm->max_value_in;
                status=DCM_PostRescaleImage(next_image,dcm,True,exception);
              }
          if (status == MagickPass)
            {
              strcpy(next_image->filename,(*image)->filename);
              next_image->scene=scene;
              if (image_list == (Image*)NULL)
                image_list=next_image;
              else
                AppendImageToList(&image_list,next_image);
            }
        }
      (void) LiberateTemporaryFile(filename);
    }
  DestroyImage(*image);
  *image=image_list;
  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d D C M I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadDCMImage reads a Digital Imaging and Communications in Medicine
%  (DICOM) file and returns it.  It It allocates the memory necessary for the
%  new Image structure and returns a pointer to the new image.
%
%  The format of the ReadDCMImage method is:
%
%      Image *ReadDCMImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadDCMImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadDCMImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    magick[MaxTextExtent];

  Image
    *image;

  int
    scene;

  size_t
    count;

  unsigned long
    status;

  DicomStream
    dcm;

  /*
    Open image file
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFail)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);

  /*
    Read DCM preamble
  */
  if ((count=ReadBlob(image,128,(char *) magick)) != 128)
    ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
  if ((count=ReadBlob(image,4,(char *) magick)) != 4)
    ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "magick: \"%.4s\"",magick);
  if (LocaleNCompare((char *) magick,"DICM",4) != 0)
    (void) SeekBlob(image,0L,SEEK_SET);

  /*
    Loop to read DCM image header one element at a time
  */
  (void) DCM_InitDCM(&dcm,image_info->verbose);
  status=DCM_ReadElement(image,&dcm,exception);
  while ((status == MagickPass) && ((dcm.group != 0x7FE0) || (dcm.element != 0x0010)))
    {
      DicomElemParseFunc
        *pfunc;

      pfunc=dicom_info[dcm.index].pfunc;
      if (pfunc != (DicomElemParseFunc *)NULL)
        status=pfunc(image,&dcm,exception);
      MagickFreeMemory(dcm.data);
      dcm.data = NULL;
      if (status == MagickPass)
        status=DCM_ReadElement(image,&dcm,exception);
    }
#if defined(IGNORE_WINDOW_FOR_UNSPECIFIED_SCALE_TYPE)
  if (dcm.rescale_type == DCM_RT_UNSPECIFIED)
    {
      dcm.window_width=0;
      dcm.rescale_slope=1;
      dcm.rescale_intercept=0;
    }
#endif
  DCM_SetRescaling(&dcm,(AccessDefinition(image_info,"dcm","avoid-scaling") != NULL));
  /*
    Now process the image data
  */
  if (status == MagickFail)
    ; 
  else
    if ((dcm.columns == 0) || (dcm.rows == 0))
      {
        ThrowException(exception,CorruptImageError,ImproperImageHeader,image->filename);
        status=MagickFail;
      }
    else if ((dcm.transfer_syntax != DCM_TS_IMPL_LITTLE) &&
             (dcm.transfer_syntax != DCM_TS_EXPL_LITTLE) &&
             (dcm.transfer_syntax != DCM_TS_EXPL_BIG) &&
             (dcm.transfer_syntax != DCM_TS_RLE))
      {
        status=DCM_ReadNonNativeImages(&image,image_info,&dcm,exception);
        dcm.number_scenes=0;
      }
    else if (dcm.rescaling != DCM_RS_POST)
      {
        status=DCM_SetupRescaleMap(image,&dcm,exception);
      }

  if (dcm.transfer_syntax == DCM_TS_RLE)
    status=DCM_ReadOffsetTable(image,&dcm,exception);

  /* Loop to process all scenes in image */
  if (status == MagickFail) dcm.number_scenes = 0;
  for (scene=0; scene < (long) dcm.number_scenes; scene++)
    {
      if (dcm.transfer_syntax == DCM_TS_RLE)
        {
          magick_uint32_t
            tag,
            length;

          /*
            Discard any remaining bytes from last fragment
          */
          if (dcm.frag_bytes)
            SeekBlob(image,dcm.frag_bytes,SEEK_CUR);

          /*
           Read fragment tag
          */
          tag=(dcm.funcReadShort(image) << 16) | dcm.funcReadShort(image);
          length=dcm.funcReadLong(image);
          if ((tag != 0xFFFEE000) || (length <= 64) || EOFBlob(image))
            {
              status=MagickFail;
              ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
              break;
            }

          /*
            Set up decompression state
          */
          dcm.frag_bytes=length;
          dcm.rle_rep_ct=0;

          /*
            Read RLE segment table
          */
          dcm.rle_seg_ct=dcm.funcReadLong(image);
          for (length=0; length < 15; length++)
            dcm.rle_seg_offsets[length]=dcm.funcReadLong(image);
          dcm.frag_bytes-=64;
          if (EOFBlob(image))
            {
              status=MagickFail;
              ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
              break;
            }
          if (dcm.rle_seg_ct > 1)
            {
              fprintf(stdout,"Multiple RLE segments in frame are not supported\n");
              status=MagickFail;
              break;
            }
        }

      /*
        Initialize image structure.
      */
      image->columns=dcm.columns;
      image->rows=dcm.rows;
      image->interlace=(dcm.interlace==1)?PlaneInterlace:NoInterlace;
#if defined(GRAYSCALE_USES_PALETTE)
      if ((image->colormap == (PixelPacket *) NULL) && (dcm.samples_per_pixel == 1))
#else
        if ((image->colormap == (PixelPacket *) NULL) && (dcm.phot_interp == DCM_PI_PALETTE_COLOR))
#endif
          {
            status=DCM_SetupColormap(image,&dcm,exception);
            if (status == MagickFail)
              break;
          }
      if (image_info->ping)
        break;

      if (CheckImagePixelLimits(image, exception) != MagickPass)
        ThrowReaderException(ResourceLimitError,ImagePixelLimitExceeded,image);

      /*
        Process image according to type
      */
      if (dcm.samples_per_pixel == 1)
        {
          if (dcm.phot_interp == DCM_PI_PALETTE_COLOR)
            status=DCM_ReadPaletteImage(image,&dcm,exception);
          else
            status=DCM_ReadGrayscaleImage(image,&dcm,exception);
        }
      else
        {
          if (image->interlace == PlaneInterlace)
            status=DCM_ReadPlanarRGBImage(image,&dcm,exception);
          else
            status=DCM_ReadRGBImage(image,&dcm,exception);
        }
      if (status != MagickPass)
        break;

      if ((dcm.rescaling == DCM_RS_PRE) &&
          ((dcm.phot_interp == DCM_PI_MONOCHROME1) ||
           (dcm.phot_interp == DCM_PI_MONOCHROME2)))
        {
          NormalizeImage(image);
        }
      else
        if (dcm.rescaling == DCM_RS_POST)
          {
            status = DCM_PostRescaleImage(image,&dcm,False,exception);
            if (status != MagickPass)
              break;
          }

      /*
        Proceed to next image.
      */
      if (image_info->subrange != 0)
        if (image->scene >= (image_info->subimage+image_info->subrange-1))
          break;
      if (scene < (long) (dcm.number_scenes-1))
        {
          /*
            Allocate next image structure.
          */
          AllocateNextImage(image_info,image);
          if (image->next == (Image *) NULL)
            {
              status=MagickFail;
              break;
            }
          image=SyncNextImageInList(image);
          status=MagickMonitorFormatted(TellBlob(image),GetBlobSize(image),
                                        exception,LoadImagesText,
                                        image->filename);
          if (status == MagickFail)
            break;
        }
    }

  /*
    Free allocated resources
  */

  if (dcm.offset_arr != NULL)
    MagickFreeMemory(dcm.offset_arr);    
  if (dcm.data != NULL)
    MagickFreeMemory(dcm.data);
#if defined(USE_GRAYMAP)
  if (dcm.graymap != (unsigned short *) NULL)
    MagickFreeMemory(dcm.graymap);
#endif
  if (dcm.rescale_map != (Quantum *) NULL)
    MagickFreeMemory(dcm.rescale_map);
  if (status == MagickPass)
    {
      while (image->previous != (Image *) NULL)
        image=image->previous;
      CloseBlob(image);
      return(image);
    }
  else
    {
      DestroyImageList(image);
      return((Image *) NULL);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r D C M I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterDCMImage adds attributes for the DCM image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterDCMImage method is:
%
%      RegisterDCMImage(void)
%
*/
ModuleExport void RegisterDCMImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("DCM");
  entry->decoder=(DecoderHandler) ReadDCMImage;
  entry->magick=(MagickHandler) IsDCM;
  entry->adjoin=False;
  entry->seekable_stream=True;
  entry->description="Digital Imaging and Communications in Medicine image";
  entry->note="See http://medical.nema.org/ for information on DICOM.";
  entry->module="DCM";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r D C M I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterDCMImage removes format registrations made by the
%  DCM module from the list of supported formats.
%
%  The format of the UnregisterDCMImage method is:
%
%      UnregisterDCMImage(void)
%
*/
ModuleExport void UnregisterDCMImage(void)
{
  (void) UnregisterMagickInfo("DCM");
}
/*
   ### TODO :
   #1 Fixes on palette support:
                - Handle palette images where # of entries > MaxColormapSize - create image
                  as Direct class, store the original palette (scaled to MaxRGB) and then map
                  input values via modified palette to output RGB values.
        - Honour palette/LUT descriptors (ie values <= min value map to first
                  entry, value = (min_value + 1) maps to second entry, and so on, whilst
                  values >= (min value + palette/LUT size) map to last entry.
   #2 Use ImportImagePixelArea?
   #3 Handling of encapsulated JPEGs which downsample to 8 bit via
      libjpeg. These lose accuracy before we can rescale to handle the
          issues of PR=1 + window center/width + rescale slope/intercept on
          MONOCHROME1 or 2. Worst case : CT-MONO2-16-chest. Currently images
          are post-rescaled based on sample range. For PseudoClass grayscales
          this is done by colormap manipulation only.
   #4 JPEG/JPEG-LS/JPEG 2000: Check that multi frame handling in DCM_ReadEncapImages
      is ok
   #5 Support LUTs?
   #6 Pixel Padding value/range - make transparent or allow use to specify a color?
*/
