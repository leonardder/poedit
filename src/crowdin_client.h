/*
 *  This file is part of Poedit (https://poedit.net)
 *
 *  Copyright (C) 2015-2023 Vaclav Slavik
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef Poedit_crowdin_client_h
#define Poedit_crowdin_client_h

#ifdef HAVE_HTTP_CLIENT

#include <memory>

#include "concurrency.h"
#include "language.h"


/**
    Client to the Crowdin platform.
 */
class CrowdinClient
{
public:
    /// Return singleton instance of the client.
    static CrowdinClient& Get();

    /// Destroys the singleton, must be called (only) on app shutdown.
    static void CleanUp();

    /// Is the user currently signed into Crowdin?
    bool IsSignedIn() const;

    /// Wrap relative Crowdin URL to absolute URL with attribution
    static std::string AttributeLink(std::string page);

    /**
        Authenticate with Crowdin.
        
        This opens the browser to authenticate the app. The app must handle
        poedit:// URL and call HandleOAuthCallback. @a callback will be
        called after receiving the OAuth token.
     */
    dispatch::future<void> Authenticate();
    void HandleOAuthCallback(const std::string& uri);
    bool IsOAuthCallback(const std::string& uri);

    /// Sign out of Crowdin, forget the token
    void SignOut();

    /// Information about logged-in user
    struct UserInfo
    {
        std::wstring name;
        std::wstring login;
        std::string avatar;
    };

    /// Retrieve information about the current user asynchronously
    dispatch::future<UserInfo> GetUserInfo();

    /// Project listing info
    struct ProjectListing
    {
        std::wstring name;
        std::string identifier;
        int id;
    };

    /// Retrieve listing of projects accessible to the user
    dispatch::future<std::vector<ProjectListing>> GetUserProjects();

    /// File information
    struct FileInfo
    {
        std::string title;
        std::string fileName, dirName, branchName;
        std::string fullPath;
        int id, dirId, branchId;
    };

    /// Project detailed information
    struct ProjectInfo
    {
        std::wstring name;
        int id;
        std::vector<Language> languages;
        std::vector<FileInfo> files;
    };

    /// Retrieve listing of projects accessible to the user
    dispatch::future<ProjectInfo> GetProjectInfo(const int project_id);

    /// Asynchronously download specific Crowdin file into @a output_file.
    dispatch::future<void> DownloadFile(int project_id,
                                        const Language& lang,
                                        int file_id,
                                        const std::string& file_extension,
                                        bool forceExportAsXliff,
                                        const std::wstring& output_file);

    /// Asynchronously upload specific Crowdin file data.
    dispatch::future<void> UploadFile(int project_id,
                                      const Language& lang,
                                      int file_id,
                                      const std::string& file_extension,
                                      const std::string& file_content);

private:
    class crowdin_http_client;
    class crowdin_token;

    CrowdinClient();
    ~CrowdinClient();

    // Initialize m_api for use with given authorization; must be called before use
    bool InitWithAuthToken(const crowdin_token& token);

    void SignInIfAuthorized();
    void SaveAndSetToken(const std::string& token);
    crowdin_token GetValidToken() const;

    mutable std::unique_ptr<crowdin_token> m_cachedAuthToken;
    std::unique_ptr<crowdin_http_client> m_api;
    std::shared_ptr<dispatch::promise<void>> m_authCallback;
    std::string m_authCallbackExpectedState;

    static CrowdinClient *ms_instance;
};

#endif // HAVE_HTTP_CLIENT

#endif // Poedit_crowdin_client_h
