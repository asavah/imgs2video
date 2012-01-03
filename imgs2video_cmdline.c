/*
  File autogenerated by gengetopt version 2.22.6
  generated with the following command:
  gengetopt -i imgs2video.ggo -a args -F imgs2video_cmdline 

  The developers of gengetopt consider the fixed text that goes in all
  gengetopt output files to be in the public domain:
  we make no copyright claims on it.
*/

/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef FIX_UNUSED
#define FIX_UNUSED(X) (void) (X) /* avoid warnings for unused params */
#endif

#include <getopt.h>

#include "imgs2video_cmdline.h"

const char *args_purpose = "Generate video from images series, with linear time transform";

const char *args_usage = "Usage: imgs2video [OPTIONS]...";

const char *args_description = "";

const char *args_help[] = {
  "  -h, --help                  Print help and exit",
  "  -V, --version               Print version and exit",
  "  -o, --output-file=filename  ",
  "  -i, --images-dir=path       ",
  "  -s, --speedup-coef=INT      How many seconds of real time fits one second of \n                                video  (default=`240')",
  "  -f, --frame-rate=INT        How many frames per second to produce  \n                                (default=`50')",
  "  -v, --vcodec=STRING         Video codec, supported are h264, flv1  \n                                (default=`h264')",
  "      --filter=STRING         avfilter arg  (default=`fifo')",
  "      --preset=STRING         H264 preset  (default=`medium')",
  "      --profile=STRING        H264 profile  (default=`baseline')",
    0
};

typedef enum {ARG_NO
  , ARG_STRING
  , ARG_INT
} cmdline_parser_arg_type;

static
void clear_given (struct args *args_info);
static
void clear_args (struct args *args_info);

static int
cmdline_parser_internal (int argc, char **argv, struct args *args_info,
                        struct cmdline_parser_params *params, const char *additional_error);

static int
cmdline_parser_required2 (struct args *args_info, const char *prog_name, const char *additional_error);

static char *
gengetopt_strdup (const char *s);

static
void clear_given (struct args *args_info)
{
  args_info->help_given = 0 ;
  args_info->version_given = 0 ;
  args_info->output_file_given = 0 ;
  args_info->images_dir_given = 0 ;
  args_info->speedup_coef_given = 0 ;
  args_info->frame_rate_given = 0 ;
  args_info->vcodec_given = 0 ;
  args_info->filter_given = 0 ;
  args_info->preset_given = 0 ;
  args_info->profile_given = 0 ;
}

static
void clear_args (struct args *args_info)
{
  FIX_UNUSED (args_info);
  args_info->output_file_arg = NULL;
  args_info->output_file_orig = NULL;
  args_info->images_dir_arg = NULL;
  args_info->images_dir_orig = NULL;
  args_info->speedup_coef_arg = 240;
  args_info->speedup_coef_orig = NULL;
  args_info->frame_rate_arg = 50;
  args_info->frame_rate_orig = NULL;
  args_info->vcodec_arg = gengetopt_strdup ("h264");
  args_info->vcodec_orig = NULL;
  args_info->filter_arg = gengetopt_strdup ("fifo");
  args_info->filter_orig = NULL;
  args_info->preset_arg = gengetopt_strdup ("medium");
  args_info->preset_orig = NULL;
  args_info->profile_arg = gengetopt_strdup ("baseline");
  args_info->profile_orig = NULL;
  
}

static
void init_args_info(struct args *args_info)
{


  args_info->help_help = args_help[0] ;
  args_info->version_help = args_help[1] ;
  args_info->output_file_help = args_help[2] ;
  args_info->images_dir_help = args_help[3] ;
  args_info->speedup_coef_help = args_help[4] ;
  args_info->frame_rate_help = args_help[5] ;
  args_info->vcodec_help = args_help[6] ;
  args_info->filter_help = args_help[7] ;
  args_info->preset_help = args_help[8] ;
  args_info->profile_help = args_help[9] ;
  
}

void
cmdline_parser_print_version (void)
{
  printf ("%s %s\n",
     (strlen(CMDLINE_PARSER_PACKAGE_NAME) ? CMDLINE_PARSER_PACKAGE_NAME : CMDLINE_PARSER_PACKAGE),
     CMDLINE_PARSER_VERSION);
}

static void print_help_common(void) {
  cmdline_parser_print_version ();

  if (strlen(args_purpose) > 0)
    printf("\n%s\n", args_purpose);

  if (strlen(args_usage) > 0)
    printf("\n%s\n", args_usage);

  printf("\n");

  if (strlen(args_description) > 0)
    printf("%s\n\n", args_description);
}

void
cmdline_parser_print_help (void)
{
  int i = 0;
  print_help_common();
  while (args_help[i])
    printf("%s\n", args_help[i++]);
}

void
cmdline_parser_init (struct args *args_info)
{
  clear_given (args_info);
  clear_args (args_info);
  init_args_info (args_info);
}

void
cmdline_parser_params_init(struct cmdline_parser_params *params)
{
  if (params)
    { 
      params->override = 0;
      params->initialize = 1;
      params->check_required = 1;
      params->check_ambiguity = 0;
      params->print_errors = 1;
    }
}

struct cmdline_parser_params *
cmdline_parser_params_create(void)
{
  struct cmdline_parser_params *params = 
    (struct cmdline_parser_params *)malloc(sizeof(struct cmdline_parser_params));
  cmdline_parser_params_init(params);  
  return params;
}

static void
free_string_field (char **s)
{
  if (*s)
    {
      free (*s);
      *s = 0;
    }
}


static void
cmdline_parser_release (struct args *args_info)
{

  free_string_field (&(args_info->output_file_arg));
  free_string_field (&(args_info->output_file_orig));
  free_string_field (&(args_info->images_dir_arg));
  free_string_field (&(args_info->images_dir_orig));
  free_string_field (&(args_info->speedup_coef_orig));
  free_string_field (&(args_info->frame_rate_orig));
  free_string_field (&(args_info->vcodec_arg));
  free_string_field (&(args_info->vcodec_orig));
  free_string_field (&(args_info->filter_arg));
  free_string_field (&(args_info->filter_orig));
  free_string_field (&(args_info->preset_arg));
  free_string_field (&(args_info->preset_orig));
  free_string_field (&(args_info->profile_arg));
  free_string_field (&(args_info->profile_orig));
  
  

  clear_given (args_info);
}


static void
write_into_file(FILE *outfile, const char *opt, const char *arg, const char *values[])
{
  FIX_UNUSED (values);
  if (arg) {
    fprintf(outfile, "%s=\"%s\"\n", opt, arg);
  } else {
    fprintf(outfile, "%s\n", opt);
  }
}


int
cmdline_parser_dump(FILE *outfile, struct args *args_info)
{
  int i = 0;

  if (!outfile)
    {
      fprintf (stderr, "%s: cannot dump options to stream\n", CMDLINE_PARSER_PACKAGE);
      return EXIT_FAILURE;
    }

  if (args_info->help_given)
    write_into_file(outfile, "help", 0, 0 );
  if (args_info->version_given)
    write_into_file(outfile, "version", 0, 0 );
  if (args_info->output_file_given)
    write_into_file(outfile, "output-file", args_info->output_file_orig, 0);
  if (args_info->images_dir_given)
    write_into_file(outfile, "images-dir", args_info->images_dir_orig, 0);
  if (args_info->speedup_coef_given)
    write_into_file(outfile, "speedup-coef", args_info->speedup_coef_orig, 0);
  if (args_info->frame_rate_given)
    write_into_file(outfile, "frame-rate", args_info->frame_rate_orig, 0);
  if (args_info->vcodec_given)
    write_into_file(outfile, "vcodec", args_info->vcodec_orig, 0);
  if (args_info->filter_given)
    write_into_file(outfile, "filter", args_info->filter_orig, 0);
  if (args_info->preset_given)
    write_into_file(outfile, "preset", args_info->preset_orig, 0);
  if (args_info->profile_given)
    write_into_file(outfile, "profile", args_info->profile_orig, 0);
  

  i = EXIT_SUCCESS;
  return i;
}

int
cmdline_parser_file_save(const char *filename, struct args *args_info)
{
  FILE *outfile;
  int i = 0;

  outfile = fopen(filename, "w");

  if (!outfile)
    {
      fprintf (stderr, "%s: cannot open file for writing: %s\n", CMDLINE_PARSER_PACKAGE, filename);
      return EXIT_FAILURE;
    }

  i = cmdline_parser_dump(outfile, args_info);
  fclose (outfile);

  return i;
}

void
cmdline_parser_free (struct args *args_info)
{
  cmdline_parser_release (args_info);
}

/** @brief replacement of strdup, which is not standard */
char *
gengetopt_strdup (const char *s)
{
  char *result = 0;
  if (!s)
    return result;

  result = (char*)malloc(strlen(s) + 1);
  if (result == (char*)0)
    return (char*)0;
  strcpy(result, s);
  return result;
}

int
cmdline_parser (int argc, char **argv, struct args *args_info)
{
  return cmdline_parser2 (argc, argv, args_info, 0, 1, 1);
}

int
cmdline_parser_ext (int argc, char **argv, struct args *args_info,
                   struct cmdline_parser_params *params)
{
  int result;
  result = cmdline_parser_internal (argc, argv, args_info, params, 0);

  if (result == EXIT_FAILURE)
    {
      cmdline_parser_free (args_info);
      exit (EXIT_FAILURE);
    }
  
  return result;
}

int
cmdline_parser2 (int argc, char **argv, struct args *args_info, int override, int initialize, int check_required)
{
  int result;
  struct cmdline_parser_params params;
  
  params.override = override;
  params.initialize = initialize;
  params.check_required = check_required;
  params.check_ambiguity = 0;
  params.print_errors = 1;

  result = cmdline_parser_internal (argc, argv, args_info, &params, 0);

  if (result == EXIT_FAILURE)
    {
      cmdline_parser_free (args_info);
      exit (EXIT_FAILURE);
    }
  
  return result;
}

int
cmdline_parser_required (struct args *args_info, const char *prog_name)
{
  int result = EXIT_SUCCESS;

  if (cmdline_parser_required2(args_info, prog_name, 0) > 0)
    result = EXIT_FAILURE;

  if (result == EXIT_FAILURE)
    {
      cmdline_parser_free (args_info);
      exit (EXIT_FAILURE);
    }
  
  return result;
}

int
cmdline_parser_required2 (struct args *args_info, const char *prog_name, const char *additional_error)
{
  int error = 0;
  FIX_UNUSED (additional_error);

  /* checks for required options */
  if (! args_info->output_file_given)
    {
      fprintf (stderr, "%s: '--output-file' ('-o') option required%s\n", prog_name, (additional_error ? additional_error : ""));
      error = 1;
    }
  
  if (! args_info->images_dir_given)
    {
      fprintf (stderr, "%s: '--images-dir' ('-i') option required%s\n", prog_name, (additional_error ? additional_error : ""));
      error = 1;
    }
  
  
  /* checks for dependences among options */

  return error;
}


static char *package_name = 0;

/**
 * @brief updates an option
 * @param field the generic pointer to the field to update
 * @param orig_field the pointer to the orig field
 * @param field_given the pointer to the number of occurrence of this option
 * @param prev_given the pointer to the number of occurrence already seen
 * @param value the argument for this option (if null no arg was specified)
 * @param possible_values the possible values for this option (if specified)
 * @param default_value the default value (in case the option only accepts fixed values)
 * @param arg_type the type of this option
 * @param check_ambiguity @see cmdline_parser_params.check_ambiguity
 * @param override @see cmdline_parser_params.override
 * @param no_free whether to free a possible previous value
 * @param multiple_option whether this is a multiple option
 * @param long_opt the corresponding long option
 * @param short_opt the corresponding short option (or '-' if none)
 * @param additional_error possible further error specification
 */
static
int update_arg(void *field, char **orig_field,
               unsigned int *field_given, unsigned int *prev_given, 
               char *value, const char *possible_values[],
               const char *default_value,
               cmdline_parser_arg_type arg_type,
               int check_ambiguity, int override,
               int no_free, int multiple_option,
               const char *long_opt, char short_opt,
               const char *additional_error)
{
  char *stop_char = 0;
  const char *val = value;
  int found;
  char **string_field;
  FIX_UNUSED (field);

  stop_char = 0;
  found = 0;

  if (!multiple_option && prev_given && (*prev_given || (check_ambiguity && *field_given)))
    {
      if (short_opt != '-')
        fprintf (stderr, "%s: `--%s' (`-%c') option given more than once%s\n", 
               package_name, long_opt, short_opt,
               (additional_error ? additional_error : ""));
      else
        fprintf (stderr, "%s: `--%s' option given more than once%s\n", 
               package_name, long_opt,
               (additional_error ? additional_error : ""));
      return 1; /* failure */
    }

  FIX_UNUSED (default_value);
    
  if (field_given && *field_given && ! override)
    return 0;
  if (prev_given)
    (*prev_given)++;
  if (field_given)
    (*field_given)++;
  if (possible_values)
    val = possible_values[found];

  switch(arg_type) {
  case ARG_INT:
    if (val) *((int *)field) = strtol (val, &stop_char, 0);
    break;
  case ARG_STRING:
    if (val) {
      string_field = (char **)field;
      if (!no_free && *string_field)
        free (*string_field); /* free previous string */
      *string_field = gengetopt_strdup (val);
    }
    break;
  default:
    break;
  };

  /* check numeric conversion */
  switch(arg_type) {
  case ARG_INT:
    if (val && !(stop_char && *stop_char == '\0')) {
      fprintf(stderr, "%s: invalid numeric value: %s\n", package_name, val);
      return 1; /* failure */
    }
    break;
  default:
    ;
  };

  /* store the original value */
  switch(arg_type) {
  case ARG_NO:
    break;
  default:
    if (value && orig_field) {
      if (no_free) {
        *orig_field = value;
      } else {
        if (*orig_field)
          free (*orig_field); /* free previous string */
        *orig_field = gengetopt_strdup (value);
      }
    }
  };

  return 0; /* OK */
}


int
cmdline_parser_internal (
  int argc, char **argv, struct args *args_info,
                        struct cmdline_parser_params *params, const char *additional_error)
{
  int c;	/* Character of the parsed option.  */

  int error = 0;
  struct args local_args_info;
  
  int override;
  int initialize;
  int check_required;
  int check_ambiguity;
  
  package_name = argv[0];
  
  override = params->override;
  initialize = params->initialize;
  check_required = params->check_required;
  check_ambiguity = params->check_ambiguity;

  if (initialize)
    cmdline_parser_init (args_info);

  cmdline_parser_init (&local_args_info);

  optarg = 0;
  optind = 0;
  opterr = params->print_errors;
  optopt = '?';

  while (1)
    {
      int option_index = 0;

      static struct option long_options[] = {
        { "help",	0, NULL, 'h' },
        { "version",	0, NULL, 'V' },
        { "output-file",	1, NULL, 'o' },
        { "images-dir",	1, NULL, 'i' },
        { "speedup-coef",	1, NULL, 's' },
        { "frame-rate",	1, NULL, 'f' },
        { "vcodec",	1, NULL, 'v' },
        { "filter",	1, NULL, 0 },
        { "preset",	1, NULL, 0 },
        { "profile",	1, NULL, 0 },
        { 0,  0, 0, 0 }
      };

      c = getopt_long (argc, argv, "hVo:i:s:f:v:", long_options, &option_index);

      if (c == -1) break;	/* Exit from `while (1)' loop.  */

      switch (c)
        {
        case 'h':	/* Print help and exit.  */
          cmdline_parser_print_help ();
          cmdline_parser_free (&local_args_info);
          exit (EXIT_SUCCESS);

        case 'V':	/* Print version and exit.  */
          cmdline_parser_print_version ();
          cmdline_parser_free (&local_args_info);
          exit (EXIT_SUCCESS);

        case 'o':	/* .  */
        
        
          if (update_arg( (void *)&(args_info->output_file_arg), 
               &(args_info->output_file_orig), &(args_info->output_file_given),
              &(local_args_info.output_file_given), optarg, 0, 0, ARG_STRING,
              check_ambiguity, override, 0, 0,
              "output-file", 'o',
              additional_error))
            goto failure;
        
          break;
        case 'i':	/* .  */
        
        
          if (update_arg( (void *)&(args_info->images_dir_arg), 
               &(args_info->images_dir_orig), &(args_info->images_dir_given),
              &(local_args_info.images_dir_given), optarg, 0, 0, ARG_STRING,
              check_ambiguity, override, 0, 0,
              "images-dir", 'i',
              additional_error))
            goto failure;
        
          break;
        case 's':	/* How many seconds of real time fits one second of video.  */
        
        
          if (update_arg( (void *)&(args_info->speedup_coef_arg), 
               &(args_info->speedup_coef_orig), &(args_info->speedup_coef_given),
              &(local_args_info.speedup_coef_given), optarg, 0, "240", ARG_INT,
              check_ambiguity, override, 0, 0,
              "speedup-coef", 's',
              additional_error))
            goto failure;
        
          break;
        case 'f':	/* How many frames per second to produce.  */
        
        
          if (update_arg( (void *)&(args_info->frame_rate_arg), 
               &(args_info->frame_rate_orig), &(args_info->frame_rate_given),
              &(local_args_info.frame_rate_given), optarg, 0, "50", ARG_INT,
              check_ambiguity, override, 0, 0,
              "frame-rate", 'f',
              additional_error))
            goto failure;
        
          break;
        case 'v':	/* Video codec, supported are h264, flv1.  */
        
        
          if (update_arg( (void *)&(args_info->vcodec_arg), 
               &(args_info->vcodec_orig), &(args_info->vcodec_given),
              &(local_args_info.vcodec_given), optarg, 0, "h264", ARG_STRING,
              check_ambiguity, override, 0, 0,
              "vcodec", 'v',
              additional_error))
            goto failure;
        
          break;

        case 0:	/* Long option with no short option */
          /* avfilter arg.  */
          if (strcmp (long_options[option_index].name, "filter") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->filter_arg), 
                 &(args_info->filter_orig), &(args_info->filter_given),
                &(local_args_info.filter_given), optarg, 0, "fifo", ARG_STRING,
                check_ambiguity, override, 0, 0,
                "filter", '-',
                additional_error))
              goto failure;
          
          }
          /* H264 preset.  */
          else if (strcmp (long_options[option_index].name, "preset") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->preset_arg), 
                 &(args_info->preset_orig), &(args_info->preset_given),
                &(local_args_info.preset_given), optarg, 0, "medium", ARG_STRING,
                check_ambiguity, override, 0, 0,
                "preset", '-',
                additional_error))
              goto failure;
          
          }
          /* H264 profile.  */
          else if (strcmp (long_options[option_index].name, "profile") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->profile_arg), 
                 &(args_info->profile_orig), &(args_info->profile_given),
                &(local_args_info.profile_given), optarg, 0, "baseline", ARG_STRING,
                check_ambiguity, override, 0, 0,
                "profile", '-',
                additional_error))
              goto failure;
          
          }
          
          break;
        case '?':	/* Invalid option.  */
          /* `getopt_long' already printed an error message.  */
          goto failure;

        default:	/* bug: option not considered.  */
          fprintf (stderr, "%s: option unknown: %c%s\n", CMDLINE_PARSER_PACKAGE, c, (additional_error ? additional_error : ""));
          abort ();
        } /* switch */
    } /* while */



  if (check_required)
    {
      error += cmdline_parser_required2 (args_info, argv[0], additional_error);
    }

  cmdline_parser_release (&local_args_info);

  if ( error )
    return (EXIT_FAILURE);

  return 0;

failure:
  
  cmdline_parser_release (&local_args_info);
  return (EXIT_FAILURE);
}
