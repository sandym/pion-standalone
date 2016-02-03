// ---------------------------------------------------------------------
// pion:  a Boost C++ framework for building lightweight HTTP interfaces
// ---------------------------------------------------------------------
// Copyright (C) 2007-2014 Splunk Inc.  (https://github.com/splunk/pion)
//
// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt
//

#ifndef __PION_ERROR_HEADER__
#define __PION_ERROR_HEADER__

#include <pion/config.hpp>
#include <string>
#include <sstream>
#include <exception>
#include <pion/utils/pion_exception.hpp>

namespace pion {    // begin namespace pion

    //
    // exception: simple exception class for pion errors that generates what()
    // strings with more descriptive messages and optionally arguments as well
    //
    class exception
        : public virtual std::exception, public virtual pion::exception_base
    {
    public:
        exception() {}
        exception(const std::string& msg) : m_what_msg(msg) {}
        exception(const char * const msg) : m_what_msg(msg) {}
        virtual ~exception() throw () {}
        virtual const char* what() const throw() {
            if (m_what_msg.empty()) update_what_msg();
            return m_what_msg.c_str();
        }
    protected:
        inline void set_what_msg(const char * const msg = NULL, const std::string * const arg1 = NULL, const std::string * const arg2 = NULL, const std::string * const arg3 = NULL) const {
            std::ostringstream tmp;
            tmp << ( msg ? msg : boost::units::detail::demangle(BOOST_EXCEPTION_DYNAMIC_TYPEID(*this).type_->name()) );
            if (arg1 || arg2 || arg3) tmp << ':';
            if (arg1) tmp << ' ' << *arg1;
            if (arg2) tmp << ' ' << *arg2;
            if (arg3) tmp << ' ' << *arg3;
            m_what_msg = tmp.str();
        }
        virtual void update_what_msg() const { set_what_msg(); }
        mutable std::string m_what_msg;
    };
    
    
    /**
     * static method that generates a meaningful diagnostic message from exceptions
     *
     * @param e reference to an exception object
     * @return std::string descriptive error message
     */
    template <class T>
    static inline std::string
    diagnostic_information( T const & e )
    {
        pion::exception_base const * const be = dynamic_cast<const pion::exception_base*>(&e);
        std::exception const * const se = dynamic_cast<const std::exception*>(&e);
        std::ostringstream tmp;
        if (se) {
            tmp << se->what();
        } else {
            tmp << boost::units::detail::demangle(BOOST_EXCEPTION_DYNAMIC_TYPEID(e).type_->name());
        }
        if (be) {
            //char const * const * fn=pion::get_error_info<pion::throw_function>(*be);
            //if (fn) tmp << " at " << *fn;
            char const * const * f=pion::get_error_info<pion::throw_file>(*be);
            if (f) {
                tmp << " [" << *f;
                if (int const * l=pion::get_error_info<pion::throw_line>(*be))
                    tmp << ':' << *l;
                tmp << "]";
            }
        }
        return tmp.str();
    }

    
    namespace error {    // begin namespace error

        //
        // pion error info types
        //
        
        /// generic error message
        typedef pion::error_info<struct errinfo_arg_name_,std::string> errinfo_message;
        
        /// name of an unrecognized configuration argument or option
        typedef pion::error_info<struct errinfo_arg_name_,std::string> errinfo_arg_name;
        
        /// file name/path
        typedef pion::error_info<struct errinfo_file_name_,std::string> errinfo_file_name;
        
        /// directory name/path
        typedef pion::error_info<struct errinfo_dir_name_,std::string> errinfo_dir_name;
        
        /// plugin identifier
        typedef pion::error_info<struct errinfo_plugin_name_,std::string> errinfo_plugin_name;
        
        /// plugin symbol name
        typedef pion::error_info<struct errinfo_dir_name_,std::string> errinfo_symbol_name;

        
        //
        // pion exception types
        //

        /// exception thrown for an invalid configuration argument or option
        class bad_arg : public pion::exception {
            virtual void update_what_msg() const {
                set_what_msg("bad argument", pion::get_error_info<errinfo_arg_name>(*this));
            }
        };
        
        /// exception thrown if there is an error parsing a configuration file
        class bad_config : public pion::exception {
            virtual void update_what_msg() const {
                set_what_msg("config parser error", pion::get_error_info<errinfo_file_name>(*this));
            }
        };
        
        /// exception thrown if we failed to open a file
        class open_file : public pion::exception {
            virtual void update_what_msg() const {
                set_what_msg("unable to open file", pion::get_error_info<errinfo_file_name>(*this));
            }
        };
        
        /// exception thrown if we are unable to open a plugin
        class open_plugin : public pion::exception {
            virtual void update_what_msg() const {
                set_what_msg("unable to open plugin", pion::get_error_info<errinfo_plugin_name>(*this));
            }
        };
        
        /// exception thrown if we failed to read data from a file
        class read_file : public pion::exception {
            virtual void update_what_msg() const {
                set_what_msg("unable to read file", pion::get_error_info<errinfo_file_name>(*this));
            }
        };
        
        /// exception thrown if a file is not found
        class file_not_found : public pion::exception {
            virtual void update_what_msg() const {
                set_what_msg("file not found", pion::get_error_info<errinfo_file_name>(*this));
            }
        };
        
        /// exception thrown if a required directory is not found
        class directory_not_found : public pion::exception {
            virtual void update_what_msg() const {
                set_what_msg("directory not found", pion::get_error_info<errinfo_dir_name>(*this));
            }
        };

        /// exception thrown if a plugin cannot be found
        class plugin_not_found : public pion::exception {
            virtual void update_what_msg() const {
                set_what_msg("plugin not found", pion::get_error_info<errinfo_plugin_name>(*this));
            }
        };
        
        /// exception thrown if we try to add or load a duplicate plugin
        class duplicate_plugin : public pion::exception {
            virtual void update_what_msg() const {
                set_what_msg("duplicate plugin", pion::get_error_info<errinfo_plugin_name>(*this));
            }
        };

        /// exception thrown if a plugin is missing a required symbol
        class plugin_missing_symbol : public pion::exception {
            virtual void update_what_msg() const {
                set_what_msg("missing plugin symbol", pion::get_error_info<errinfo_symbol_name>(*this));
            }
        };
      
        /// exception thrown if a plugin has an undefined state
        class plugin_undefined : public pion::exception {
            virtual void update_what_msg() const {
                set_what_msg("plugin has undefined state");
            }
        };
        
        /// exception thrown if a bad password hash is provided
        class bad_password_hash : public pion::exception {
            virtual void update_what_msg() const {
                set_what_msg("bad password hash");
            }
        };
    
    }   // end namespace error
    
}   // end namespace pion

#endif
