/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <xapian.h>
#include "index.h"
#include "main.h"
#include "util.h"

CIndex::CIndex ( std::string dbpath_ ) :
	dbpath ( dbpath_ )
{
}

std::vector < std::string > CIndex::Search ( std::string search_, int* size_ )
{
	std::vector < std::string > output;
	
// 	if ( search_.length() - search_.find_last_of(" ") > 5 && search_.length() > 4 )
// 		search_ += "*";
	
	// Catch any Xapian::Error exceptions thrown
	try {
		// Make the database
		Xapian::Database db ( dbpath );
		
		// Start an enquire session
		Xapian::Enquire enquire(db);
		
		// Build the query object
		Xapian::QueryParser parser;
		parser.set_database(db);
		parser.set_default_op(Xapian::Query::OP_AND);

		Xapian::Query query = parser.parse_query ( search_, Xapian::QueryParser::FLAG_WILDCARD );
		std::cout << "Performing query `" << query.get_description() << "'" << std::endl;
		
		// Give the query object to the enquire session
		enquire.set_query ( query );
		
		// Get the top results of the query
		Xapian::MSet matches = enquire.get_mset ( 0, *size_ );
		
		// Display the results
		std::cout << matches.size() << " results found" << std::endl;
		*size_ = matches.size();
		
		for ( Xapian::MSetIterator i = matches.begin(); i != matches.end(); ++i ) {
			Xapian::Document doc = i.get_document();
			
			output.push_back ( doc.get_data() );
		}
		
	} catch ( const Xapian::Error &error ) {
		std::cout << "Exception: "  << error.get_msg() << std::endl;
	}
	
	return output;
}

int performSearch ( void* p ) {
	int size = 7;
	
	GetWizipedia()->GetGui()->SetSearchResults ( GetWizipedia()->GetIndex()->Search ( *(std::string*)(p), &size ), &size );
	
	
}
