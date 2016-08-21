Drupal.behaviors.membership_admin = function(context) {
  $('body:not(.set_logo_click_handler)').addClass('set_logo_click_handler').each(function() {
    //Remove Logo Click Handler
    $('#remove_logo').click(function(){
      var account_id_in = ACCOUNT_ID;

      var post_data = {
        account_id : account_id_in
      };

      $.post(BASE_URL + 'rsaccountadmin/edit/remove_image/' + account_id_in, post_data, (function(account_id_in) {
        //Removing logo image and asscociated button
        $('#rsaccountadmin-account-edit-form div').find('img').remove();
        $('#remove_logo').remove();
      }), 'json');

    });
  });
};


