#pragma once

#include <core/factory.hpp>
#include <core/delegate.hpp>

namespace wee {

	class ad;


	class ad {
	public:
		virtual ~ad() = default;
		virtual void fetch() = 0;
		virtual void show() = 0;
		virtual void dismiss() = 0;
		virtual bool is_ready() = 0;
		virtual const std::string& get_publisher_id() const = 0;
		virtual const std::string& get_app_id() const = 0;
		virtual void set_publisher_id(const std::string&) = 0;
		virtual void set_app_id(const std::string&) = 0;
	public:
		event_handler<void(const ad&)> was_fetched;
		event_handler<void(const ad&)> was_shown;
		event_handler<void(const ad&)> was_hidden;
		event_handler<void(const ad&)> was_dismissed;
		//    event_handler<void(const AdEventArgs&)> onAdIsReady;
	};

	typedef factory<ad> ad_factory;

	class ad_manager : public singleton<ad_manager> {
		DISALLOW_COPY_AND_ASSIGN(ad_manager);
	public:
		ad_manager() {}
		~ad_manager() {}
		void cache(std::istream &is);
		ad* at(const std::string& name);
	};

} 

