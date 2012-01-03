/** @file imgs2video_cmdline.h
 *  @brief The header file for the command line option parser
 *  generated by GNU Gengetopt version 2.22.6
 *  http://www.gnu.org/software/gengetopt.
 *  DO NOT modify this file, since it can be overwritten
 *  @author GNU Gengetopt by Lorenzo Bettini */

#ifndef IMGS2VIDEO_CMDLINE_H
#define IMGS2VIDEO_CMDLINE_H

/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h> /* for FILE */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef CMDLINE_PARSER_PACKAGE
/** @brief the program name (used for printing errors) */
#define CMDLINE_PARSER_PACKAGE "imgs2video"
#endif

#ifndef CMDLINE_PARSER_PACKAGE_NAME
/** @brief the complete program name (used for help and version) */
#define CMDLINE_PARSER_PACKAGE_NAME "imgs2video"
#endif

#ifndef CMDLINE_PARSER_VERSION
/** @brief the program version */
#define CMDLINE_PARSER_VERSION "1.0"
#endif

/** @brief Where the command line options are stored */
struct args
{
  const char *help_help; /**< @brief Print help and exit help description.  */
  const char *version_help; /**< @brief Print version and exit help description.  */
  char * output_file_arg;	/**< @brief .  */
  char * output_file_orig;	/**< @brief  original value given at command line.  */
  const char *output_file_help; /**< @brief  help description.  */
  char * images_dir_arg;	/**< @brief .  */
  char * images_dir_orig;	/**< @brief  original value given at command line.  */
  const char *images_dir_help; /**< @brief  help description.  */
  int speedup_coef_arg;	/**< @brief How many seconds of real time fits one second of video (default='240').  */
  char * speedup_coef_orig;	/**< @brief How many seconds of real time fits one second of video original value given at command line.  */
  const char *speedup_coef_help; /**< @brief How many seconds of real time fits one second of video help description.  */
  int frame_rate_arg;	/**< @brief How many frames per second to produce (default='50').  */
  char * frame_rate_orig;	/**< @brief How many frames per second to produce original value given at command line.  */
  const char *frame_rate_help; /**< @brief How many frames per second to produce help description.  */
  char * vcodec_arg;	/**< @brief Video codec, supported are h264, flv1 (default='h264').  */
  char * vcodec_orig;	/**< @brief Video codec, supported are h264, flv1 original value given at command line.  */
  const char *vcodec_help; /**< @brief Video codec, supported are h264, flv1 help description.  */
  char * filter_arg;	/**< @brief avfilter arg (default='fifo').  */
  char * filter_orig;	/**< @brief avfilter arg original value given at command line.  */
  const char *filter_help; /**< @brief avfilter arg help description.  */
  char * preset_arg;	/**< @brief H264 preset (default='medium').  */
  char * preset_orig;	/**< @brief H264 preset original value given at command line.  */
  const char *preset_help; /**< @brief H264 preset help description.  */
  char * profile_arg;	/**< @brief H264 profile (default='baseline').  */
  char * profile_orig;	/**< @brief H264 profile original value given at command line.  */
  const char *profile_help; /**< @brief H264 profile help description.  */
  
  unsigned int help_given ;	/**< @brief Whether help was given.  */
  unsigned int version_given ;	/**< @brief Whether version was given.  */
  unsigned int output_file_given ;	/**< @brief Whether output-file was given.  */
  unsigned int images_dir_given ;	/**< @brief Whether images-dir was given.  */
  unsigned int speedup_coef_given ;	/**< @brief Whether speedup-coef was given.  */
  unsigned int frame_rate_given ;	/**< @brief Whether frame-rate was given.  */
  unsigned int vcodec_given ;	/**< @brief Whether vcodec was given.  */
  unsigned int filter_given ;	/**< @brief Whether filter was given.  */
  unsigned int preset_given ;	/**< @brief Whether preset was given.  */
  unsigned int profile_given ;	/**< @brief Whether profile was given.  */

} ;

/** @brief The additional parameters to pass to parser functions */
struct cmdline_parser_params
{
  int override; /**< @brief whether to override possibly already present options (default 0) */
  int initialize; /**< @brief whether to initialize the option structure args (default 1) */
  int check_required; /**< @brief whether to check that all required options were provided (default 1) */
  int check_ambiguity; /**< @brief whether to check for options already specified in the option structure args (default 0) */
  int print_errors; /**< @brief whether getopt_long should print an error message for a bad option (default 1) */
} ;

/** @brief the purpose string of the program */
extern const char *args_purpose;
/** @brief the usage string of the program */
extern const char *args_usage;
/** @brief all the lines making the help output */
extern const char *args_help[];

/**
 * The command line parser
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser (int argc, char **argv,
  struct args *args_info);

/**
 * The command line parser (version with additional parameters - deprecated)
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @param override whether to override possibly already present options
 * @param initialize whether to initialize the option structure my_args_info
 * @param check_required whether to check that all required options were provided
 * @return 0 if everything went fine, NON 0 if an error took place
 * @deprecated use cmdline_parser_ext() instead
 */
int cmdline_parser2 (int argc, char **argv,
  struct args *args_info,
  int override, int initialize, int check_required);

/**
 * The command line parser (version with additional parameters)
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @param params additional parameters for the parser
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_ext (int argc, char **argv,
  struct args *args_info,
  struct cmdline_parser_params *params);

/**
 * Save the contents of the option struct into an already open FILE stream.
 * @param outfile the stream where to dump options
 * @param args_info the option struct to dump
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_dump(FILE *outfile,
  struct args *args_info);

/**
 * Save the contents of the option struct into a (text) file.
 * This file can be read by the config file parser (if generated by gengetopt)
 * @param filename the file where to save
 * @param args_info the option struct to save
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_file_save(const char *filename,
  struct args *args_info);

/**
 * Print the help
 */
void cmdline_parser_print_help(void);
/**
 * Print the version
 */
void cmdline_parser_print_version(void);

/**
 * Initializes all the fields a cmdline_parser_params structure 
 * to their default values
 * @param params the structure to initialize
 */
void cmdline_parser_params_init(struct cmdline_parser_params *params);

/**
 * Allocates dynamically a cmdline_parser_params structure and initializes
 * all its fields to their default values
 * @return the created and initialized cmdline_parser_params structure
 */
struct cmdline_parser_params *cmdline_parser_params_create(void);

/**
 * Initializes the passed args structure's fields
 * (also set default values for options that have a default)
 * @param args_info the structure to initialize
 */
void cmdline_parser_init (struct args *args_info);
/**
 * Deallocates the string fields of the args structure
 * (but does not deallocate the structure itself)
 * @param args_info the structure to deallocate
 */
void cmdline_parser_free (struct args *args_info);

/**
 * Checks that all the required options were specified
 * @param args_info the structure to check
 * @param prog_name the name of the program that will be used to print
 *   possible errors
 * @return
 */
int cmdline_parser_required (struct args *args_info,
  const char *prog_name);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* IMGS2VIDEO_CMDLINE_H */
