#include <fstream>

class Log {
public:
	static void  write_ts_to_log_file( const std::string &text )
	{
	    std::ofstream log_file(
	        "/home/vartika-kite/ndn-kite-master/results/res/kite_ts_log_file.txt",
	        std::ios_base::out | std::ios_base::app );
	    log_file << text << std::endl;
	}

	static void  write_to_tracing_interest_tracker_node_n( const std::string &text , const char* fileName)
	{
		std::ofstream log_file(
				fileName,
				std::ios_base::out | std::ios_base::trunc );
		log_file << text << std::endl;
	}
	static void  write_to_traced_interest_tracker_node_n( const std::string &text , const char* fileName)
	{
		std::ofstream log_file(
				fileName,
				std::ios_base::out | std::ios_base::trunc );
		log_file << text << std::endl;
	}
	//20151001
	static void write_to_on_data_tracker(const std::string& text, const char* fileName)
	{
		std::ofstream log_file(fileName, std::ios_base::out | std::ios_base::trunc);
		log_file << text << std::endl;
	}

	/*static void  write_to_tracing_interest_tracker( const std::string &text )
	{
		std::ofstream log_file(
				"/home/vartika-kite/kite_log_tracing.txt",
				std::ios_base::out | std::ios_base::trunc );
		log_file << text << std::endl;
	}
	static void  write_to_traced_interest_tracker( const std::string &text )
	{
		std::ofstream log_file(
				"/home/vartika-kite/kite_log_traced.txt",
				std::ios_base::out | std::ios_base::trunc );
		log_file << text << std::endl;
	}*/

};

