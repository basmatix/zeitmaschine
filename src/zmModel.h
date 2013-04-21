/// copyright (C) 2013 Frans Fürst
/// -*- coding: utf-8 -*-

#ifndef FLOWMODEL_H
#define FLOWMODEL_H

#include "zmThing.h"
#include "zmChangeSet.h"

#include <map>
#include <string>

namespace zm
{
    class ThingsModel
    {
    public:

        typedef std::map< std::string, Thing * > ThingsModelMapType;

    private:

        ThingsModelMapType  m_things;
        ThingsModelMapType  m_thingsOnLoad;
        std::string         m_localFolder;
        std::string         m_filename;
        ChangeSet           m_changeSet;

        std::string         m_temporaryJournalFile;
        bool                m_initialized;
        bool                m_dirty;  // maybe we need more

    public:

        ThingsModel();

        /// sets the one and only local folder for configuration temorary and
        /// snapshot files
        void setLocalFolder( const std::string &path );

        /// defines a (externally synced) folder exchange with other clients
        /// and/or users
        void addDomainSyncFolder( const std::string &domainName, const std::string &path );

        /// will initialize the model by loading the persistant state using
        /// the given information
        void initialize();

        /// will write recent changes to the journal file
        void localSave();

        /// make a fulf model sync.
        /// first load and apply new journal files in the sync folders. Then
        /// write the local model files and make the temporary journal file
        /// available to the sync folders and
        void sync();

        /// returns whether there is a valid username stored
        bool hasUsedUsername() const;

        /// returns whether there is a valid hostname stored
        bool hasUsedHostname() const;

        /// returns the username used by us
        std::string getUsedUsername() const;

        /// set the username used by us - should be the system user
        /// name but doesn't have to
        void setUsedUsername( const std::string &username );

        /// returns the hostname used by us - should be the system user
        /// name but doesn't have to
        std::string getUsedHostname() const;

        /// set the hostname used by us
        void setUsedHostname( const std::string &hostname );

    private:

        void load( const std::string &filename );

        void dirty();

        void save( const std::string &filename );

    /// const interface
    public:

        const ThingsModelMapType & things() const;

        bool equals( const ThingsModelMapType &thingsMap, const ThingsModelMapType &thingsMapOther ) const;

        size_t getItemCount() const;

        std::time_t getCreationTime( const std::string &uid ) const;

        bool hasItem( const std::string &uid ) const;

        std::string getValue( const std::string &uid, const std::string &name ) const;

        const std::string & getCaption( const std::string &uid ) const;

        bool hasAttribute( const std::string uid, const std::string attribute ) const;

        bool hasValue( const std::string uid, const std::string name ) const;

        bool itemContentMatchesString( const std::string &uid, const std::string &searchString ) const;

    /// write relevant interface
    public:

        std::string createNewItem( const std::string &caption );

        void eraseItem( const std::string &uid );

        void addAttribute( const std::string &uid, const std::string &attribute );

        bool removeAttribute( const std::string &uid, const std::string &attribute );

        void setValue( const std::string &uid, const std::string &name, const std::string &value );

        void setCaption( const std::string &uid, const std::string &caption );

    private:

        // returns 16x8 bit
        static inline std::string generateUid()
        {
            std::string l_return;
            l_return.reserve(16);
            l_return.resize(16);
            for( int i = 0; i < 16; ++i )
            {
                const char *l_characters = "0123456789abcdef";
                l_return[i] = l_characters[ rand() % 16 ];
            }
            return l_return;
        }

        static void clear( ThingsModelMapType &thingsMap );

        static void yamlToThingsMap( YAML::Node yamlNode, ThingsModelMapType &thingsMap );
    };
}

#endif
