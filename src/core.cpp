#include <sstream>
#include <fstream>

#include "core.h"

const var_info var_info_invalid = {	0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

static const char *var_data_types[] = 
{	"<invalid>", // SSC_INVALID
	"<string>",  // SSC_STRING
	"<number>",  // SSC_NUMBER
	"<array>",   // SSC_ARRAY
	"<matrix>",  // SSC_MATRIX
	NULL };

const char *var_data::type_name()
{
	if (type >= 0 && type < 5) return var_data_types[ (int)type ];
	else return NULL;
}

std::string var_data::type_name(int type)
{
	if (type >= 0 && type < 5) return var_data_types[ (int)type ];
	else return "";
}

var_table::var_table()
{
	/* nothing to do here */
}

var_table::~var_table()
{
	var_hash::iterator it;
	for ( it = m_hash.begin(); it !=m_hash.end(); ++it )
		delete it->second; // delete the var_data object
}

void var_table::assign( const std::string &name, const var_data &val )
{
	var_data *v = lookup(name);
	if (!v)
	{
		v = new var_data;
		m_hash[ util::lower_case(name) ] = v;
	}
	
	*v = val;
}

void var_table::unassign( const std::string &name )
{
	var_hash::iterator it = m_hash.find( util::lower_case(name) );
	if (it != m_hash.end())
	{
		delete (*it).second; // delete the associated data
		m_hash.erase( it );
	}
}

var_data *var_table::lookup( const std::string &name )
{
	var_hash::iterator it = m_hash.find( util::lower_case(name) );
	if ( it != m_hash.end() )
		return (*it).second;
	else
		return NULL;
}

compute_module::compute_module( )
	:  m_infomap(NULL), m_handler(NULL), m_vartab(NULL)
{
	/* nothing to do */
}

compute_module::~compute_module()
{
	if (m_infomap) delete m_infomap;
}

void compute_module::set_param_info( param_info *plist /* last one 'name' must be null, static array */ )
{
	m_paramlist.clear();
	int i=0;
	while (plist[i].name != NULL && plist[i].type != SSC_INVALID)
	{
		if (plist[i].type == SSC_STRING ||
			plist[i].type == SSC_NUMBER )
		{
			m_paramlist.push_back( &plist[i] );
		}
		i++;
	}
}

param_info *compute_module::get_param_info(int index)
{
	if (index >= 0 && index < (int) m_paramlist.size() )
		return m_paramlist[index];
	else
		return NULL;
}

void compute_module::set_param( const std::string &name, const std::string &value )
{
	m_paramtab.assign( name, var_data(value) );
}

void compute_module::set_param( const std::string &name, ssc_number_t value )
{
	m_paramtab.assign( name, var_data(value) );
}

ssc_number_t compute_module::param_number( const std::string &name ) throw( general_error )
{
	var_data *t = m_paramtab.lookup( name );
	if (!t) throw general_error( "request for unassigned compute module parameter: '" + name + "'" );
	if (t->type != SSC_NUMBER) throw general_error( "compute module parameter '" + name + "' has type " + var_data::type_name(t->type) + ", not <number> as requested" );
	return t->num;
}

std::string compute_module::param_string( const std::string &name ) throw( general_error )
{
	var_data *t = m_paramtab.lookup( name );
	if (!t) throw general_error( "request for unassigned compute module parameter: '" + name + "'" );
	if (t->type != SSC_STRING) throw general_error( "compute module parameter '" + name + "' has type " + var_data::type_name(t->type) + ", not <string> as requested" );
	return t->str;
}

bool compute_module::compute( handler_interface *handler, var_table *data )
{
	m_handler = NULL;
	m_vartab = NULL;

	if (!handler)
	{
		log("no request handler assigned to computation engine", SSC_ERROR);
		return false;
	}
	m_handler = handler;

	if (!data)
	{
		log("no data object assigned to computation engine", SSC_ERROR);
		return false;
	}
	m_vartab = data;

	if (m_varlist.size() == 0)
	{
		log("no variables defined for computation engine", SSC_ERROR);
		return false;
	}
	
	try { // catch any 'general_error' that can be thrown during precheck, exec, and postcheck

		if (!verify("precheck input", SSC_INPUT)) return false;
		if (!exec()) return false; // pure virtual, implemented in descendants
		if (!verify("postcheck output", SSC_OUTPUT)) return false;

	} catch ( general_error e )	{
		log( e.err_text, SSC_ERROR, e.time );
		return false;
	}
	
	return true;
}

bool compute_module::verify(const std::string &phase, int check_var_type) throw( general_error )
{
	std::vector< var_info* >::iterator it;
	for (it=m_varlist.begin();it!=m_varlist.end();++it)
	{
		var_info *vi = *it;
		if ( vi->var_type == check_var_type
			|| vi->var_type == SSC_INOUT )
		{
			if ( check_required( vi->name ) )
			{
				// if the variable is required, make sure it exists
				// and that it is of the correct data type
				var_data *dat = lookup( vi->name );
				if (!dat)
				{
					log(phase + ": variable '" + std::string(vi->name) + "' required but not assigned");
					return false;
				}
				else if (dat->type != vi->data_type)
				{
					log(phase + ": variable '" + std::string(vi->name) + "' (" + var_data::type_name(dat->type) + ") of wrong type, " + var_data::type_name( vi->data_type ) + " required.");
					return false;
				}

				// now check constraints on it
				std::string fail_text;
				if (!check_constraints( vi->name, fail_text ))
				{
					log(fail_text, SSC_ERROR);
					return false;
				}
			}
		}
	}

	return true;
}

void compute_module::add_var_info( var_info vi[] )
{
	int i=0;
	while ( vi[i].data_type != SSC_INVALID
		&& vi[i].name != NULL )
	{
		m_varlist.push_back( &vi[i] );
		i++;
	}
}

void compute_module::build_info_map()
{
	if (m_infomap) delete m_infomap;

	m_infomap = new unordered_map<std::string, var_info*>;
	
	std::vector<var_info*>::iterator it;
	for (it = m_varlist.begin(); it != m_varlist.end(); ++it)
		(*m_infomap)[ (*it)->name ] = *it;
}

void compute_module::update( const std::string &current_action, float percent_done, float time )
{
	// forward to handler interface
	if (m_handler) m_handler->on_update( current_action, percent_done, time);
}

void compute_module::log( const std::string &msg, int type, float time )
{
	// forward to handler interface
	if (m_handler) m_handler->on_log( msg, type, time );

	// also save it in module object
	m_loglist.push_back( log_item( type, msg, time ) );
}

void compute_module::clear_log()
{
	m_loglist.clear();
}

bool compute_module::extproc( const std::string &command, const std::string &workdir )
{
	if (m_handler) return m_handler->on_exec( command, workdir);
	else return false;
}

compute_module::log_item *compute_module::log(int index)
{
	if (index >= 0 && index < (int)m_loglist.size())
		return &m_loglist[index];
	else 
		return NULL;
}
	
var_info *compute_module::info(int index)
{
	if (index >= 0 && index < (int)m_varlist.size())
		return m_varlist[index];
	else
		return NULL;
}

const var_info &compute_module::info( const std::string &name ) throw( general_error )
{
	// if there is an info lookup table, use it
	if (m_infomap != NULL)
	{
		unordered_map<std::string, var_info*>::iterator pos = m_infomap->find(name);
		if (pos != m_infomap->end())
			return (*(pos->second));
	}

	// otherwise search
	std::vector< var_info* >::iterator it;
	for (it = m_varlist.begin(); it != m_varlist.end(); ++it)
	{
		if ( (*it)->name == name )
			return *(*it);
	}

	throw general_error("variable information lookup fail: '" + name + "'");
}

void compute_module::assign( const std::string &name, const var_data &value ) throw( general_error )
{
	var_data *v = lookup(name);
	if (v) v->copy(value);
}

var_data *compute_module::lookup( const std::string &name ) throw( general_error )
{
	if (!m_vartab) throw general_error("invalid data container object reference");
	return m_vartab->lookup(name);
}

var_data &compute_module::value( const std::string &name ) throw( general_error )
{
	var_data *v = lookup( name );
	if (!v)	throw general_error("undefined variable access: '" + name + "'");
	return (*v);
}

int compute_module::as_integer( const std::string &name ) throw( general_error )
{
	var_data &x = value(name);
	if (x.type != SSC_NUMBER) throw cast_error("integer", x, name);
	return (int) x.num;
}

bool compute_module::as_boolean( const std::string &name ) throw( general_error )
{
	var_data &x = value(name);
	if (x.type != SSC_NUMBER) throw cast_error("boolean", x, name);
	return (bool) ( (int)(x.num!=0) );
}

float compute_module::as_float( const std::string &name ) throw( general_error )
{
	var_data &x = value(name);
	if (x.type != SSC_NUMBER) throw cast_error("float", x, name);
	return (float) x.num;
}

double compute_module::as_double( const std::string &name ) throw( general_error )
{
	var_data &x = value(name);
	if (x.type != SSC_NUMBER) throw cast_error("double", x, name);
	return (double) x.num;
}

const char *compute_module::as_string( const std::string &name ) throw( general_error )
{
	var_data &x = value(name);
	if (x.type != SSC_STRING) throw cast_error("string", x, name);
	return x.str.c_str();
}

ssc_number_t *compute_module::as_array( const std::string &name, size_t *count ) throw( general_error )
{
	var_data &x = value(name);
	if (x.type != SSC_ARRAY) throw cast_error("array", x, name);
	if (count) *count = x.num.length();
	return x.num.data();
}

ssc_number_t *compute_module::as_matrix( const std::string &name, size_t *rows, size_t *cols ) throw( general_error )
{
	var_data &x = value(name);
	if (x.type != SSC_MATRIX) throw cast_error("matrix", x, name);
	if (rows) *rows = x.num.nrows();
	if (cols) *cols = x.num.ncols();
	return x.num.data();
}

ssc_number_t compute_module::get_operand_value( const std::string &input, const std::string &cur_var_name) throw( general_error )
{	
	if (input.length() < 1) throw check_error(cur_var_name, "input is null to get_operand_value", input);

	if (isalpha(input[0]))
	{
		var_data *v = lookup(input);
		if (!v) throw check_error(cur_var_name, "unassigned referenced",  input );
		if (v->type != SSC_NUMBER) throw check_error(cur_var_name, "number type required", input );
		return v->num;
	}
	else
	{
		double x = 0;
		if (!util::to_double( input, &x )) throw check_error(cur_var_name, "number conversion", input );
		return (ssc_number_t) x;
	}
}

bool compute_module::check_required( const std::string &name ) throw( general_error )
{
	// only check if the variable is required as input to the simulation context
	// if it is an input or an inout variable

	const var_info &inf = info(name);
	if (inf.var_type != SSC_INPUT ||
		inf.var_type != SSC_INOUT ||
		inf.required_if == NULL)
		return false;

	std::string reqexpr = inf.required_if;

	if (reqexpr == "*")
	{
		return true; // Always required
	}
	else if (reqexpr == "?")
	{
		return false; // Always optional
	}
	else
	{
		// run tests
		std::string::size_type pos = std::string::npos;
		std::vector< std::string > expr_list = util::split(util::lower_case(reqexpr), "&|", true, true );
		
		int cur_result = -1;
		char cur_cond_oper = 0;
		for ( std::vector< std::string >::iterator it = expr_list.begin(); it != expr_list.end(); ++it )
		{
			std::string expr = *it;
			if (expr == "&")
			{
				cur_cond_oper = '&';
				continue;
			}
			else if (expr == "|")
			{
				cur_cond_oper = '|';
				continue;
			}
			else
			{
				int expr_result = 0;
				char op = 0;
				if ( (pos=expr.find('=')) != std::string::npos ) op = '=';
				else if ( (pos=expr.find('<')) != std::string::npos ) op = '<';
				else if ( (pos=expr.find('>')) != std::string::npos ) op = '>';
				else if ( (pos=expr.find(':')) != std::string::npos ) op = ':';

				if (!op) throw check_error(name, "invalid operator", expr );

				std::string lhs = expr.substr(0, pos);
				std::string rhs = expr.substr(pos+1);
				
				if (lhs.length() < 1 || rhs.length() < 1) throw check_error(name, "null lhs or rhs in subexpr", expr);

				if (op == ':')
				{
					/* handle built-in test operators */

					if (lhs == "na") // check if 'rhs' is not-assigned
					{
						expr_result = lookup(rhs)==NULL ? 1 : 0;
					}
					else if (lhs == "a")
					{
						expr_result = lookup(rhs)!=NULL ? 1 : 0;
					}
					else
					{
						throw check_error(name, "invalid built-in test", expr);
					}
				}
				else
				{
					ssc_number_t lhs_val = get_operand_value(lhs,name);
					ssc_number_t rhs_val = get_operand_value(rhs,name);

					switch(op)
					{
					case '=': expr_result = lhs_val == rhs_val ? 1 : 0 ; break;
					case '<': expr_result = lhs_val < rhs_val ? 1 : 0 ; break;
					case '>': expr_result = lhs_val > rhs_val ? 1 : 0 ; break;
					default: throw check_error(name, "invalid numerical operator", expr);
					}
				}

				if (cur_result < 0)
				{
					cur_result = expr_result;
				}
				else if (cur_cond_oper == '&')
				{
					cur_result = (cur_result && expr_result);
				}
				else if (cur_cond_oper == '|')
				{
					cur_result = (cur_result || expr_result);
				}
				else
					throw check_error(name, "invalid evaluation sequence", reqexpr);
			}
		}

		return cur_result != 0 ? true : false;
	}
	
	return false;
}

bool compute_module::check_constraints( const std::string &name, std::string &fail_text) throw( general_error )
{
#define fail_constraint( str ) { fail_text = "fail("+name+", "+expr+"): "+std::string(str); return false; }

	const var_info &inf = info(name);

	if (inf.constraints == NULL) return true; // pass if no constraints defined

	var_data &dat = value(name);
	
	std::vector< std::string > exprlist = util::split( inf.constraints, "," );
	for ( std::vector<std::string>::iterator it=exprlist.begin(); it!=exprlist.end(); ++it )
	{
		std::string::size_type pos;
		std::string expr = util::lower_case(*it);
		if (expr == "tmyepw")
		{
			if (dat.type != SSC_STRING || dat.str.length() <= 4)
				fail_constraint("string data type required with length greater than 4 chars: " + dat.str);

			std::string ext = dat.str.substr( dat.str.length()-3 );
			if (ext != "tm2" || ext != "tm3" || ext != "epw" || ext != "csv")
				fail_constraint("file extension was not tm2,tm3,epw,csv: " + ext);
		}
		else if (expr == "local_file")
		{
			if (dat.type != SSC_STRING)
				fail_constraint("string data type required");

			std::ifstream f_in( dat.str.c_str(), std::ios_base::in );
			if (f_in.is_open())
				f_in.close();
			else
				fail_constraint("could not open for read: '" + dat.str + "'");
		}
		else if (expr == "mxh_schedule")
		{
			if (dat.type != SSC_STRING)
				fail_constraint("string data type required");

			if (dat.str.length() != 288)
				fail_constraint( "288 characters required (24x12) but " + util::to_string((int)dat.str.length()) + " found" );
			
			for ( std::string::size_type i=0;i<dat.str.length(); i++)
				if ( dat.str[i] < '0' || dat.str[i] > '9' ) 
					fail_constraint( util::format("invalid character %c at %d", (char)dat.str[i], (int)i) );
		}
		else if (expr == "boolean")
		{
			if (dat.type != SSC_NUMBER)
				fail_constraint("number data type required");

			int val = (int)dat.num;
			if (val != 0 || val != 1)
				fail_constraint("value was not 0 nor 1");
		}
		else if (expr == "integer")
		{
			if (dat.type != SSC_NUMBER)
				fail_constraint("number data type required");

			if ( ((ssc_number_t)((int)dat.num)) != dat.num )
				fail_constraint("number could not be interpreted as an integer: " + util::to_string( (double) dat.num ));
		}
		else if ( (pos=expr.find('=')) != std::string::npos )
		{
			std::string test = expr.substr(0, pos);
			std::string rhs = expr.substr(pos+1);

			if (test == "min")
			{
				if (dat.type != SSC_NUMBER) throw constraint_error(name, "cannot test for min with non-numeric type", expr);
				double minval = 0;
				if (!util::to_double( rhs, &minval )) throw constraint_error(name, "test for min requires a number value", expr);
				if ( dat.num < (ssc_number_t)minval )
					fail_constraint( util::to_string( (double)dat.num ) );
			}
			else if (test == "max")
			{
				if (dat.type != SSC_NUMBER) throw constraint_error(name, "cannot test for max with non-numeric type", expr);
				double maxval = 0;
				if (!util::to_double( rhs, &maxval )) throw constraint_error(name, "test for max requires a numeric value", expr);
				if (dat.num > (ssc_number_t)maxval )
					fail_constraint( util::to_string( (double)dat.num ) );
			}
			else if (test == "length")
			{
				if (dat.type != SSC_ARRAY) throw constraint_error(name, "cannot test for length with non-array type", expr);
				int lenval = 0;
				if (!util::to_integer( rhs, &lenval )) throw constraint_error(name, "test for length requires an integer value", expr);
				size_t len = (size_t)lenval;
				if (dat.num.length() != len)
					fail_constraint( util::to_string( (int)dat.num.length() ) );
			}
			else if (test == "length_multiple_of")
			{
				if (dat.type != SSC_ARRAY) throw constraint_error(name, "cannot test for length_multiple_of with non-array type", expr);
				int lenval = 0;
				if (!util::to_integer( rhs, &lenval ) || lenval < 1) throw constraint_error(name, "test for length_multiple_of requires a positive integer value", expr);
				size_t len = (size_t)lenval;
				size_t multiplier = dat.num.length() / len;
				if ( dat.num.length() < len || len*multiplier != dat.num.length() )
					fail_constraint( util::to_string( (int)dat.num.length() ) );
			}
		}
		else
		{
			throw constraint_error( name, "invalid test or expression", expr );
		}

	}

	// all constraints passed fine
	return true;

#undef fail_constraint
}
